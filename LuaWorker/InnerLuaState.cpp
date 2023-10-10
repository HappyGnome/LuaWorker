/*****************************************************************************\
*
*  Copyright 2023 HappyGnome
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http ://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
\*****************************************************************************/

#include "InnerLuaState.h"
#include "Worker.h"
#include "LuaCancellationException.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include <chrono>

using namespace std::chrono_literals;
using std::chrono::system_clock;

using namespace LuaWorker;

const char* InnerLuaState::cLuaStateHandleKey = "_LUAWORKER_STATE";
const char* InnerLuaState::cInLuaWorkerTableName = "InLuaWorker";
const char* InnerLuaState::cInLuaWorkerThreadsTableName = "_Threads";

//----------------------
// Private
//----------------------

bool InnerLuaState::HandleSuspendedTask(lua_State *pThread, std::chrono::duration<float> &resumeAfter)
{
	if (mLua == nullptr) return false;

	resumeAfter = mSuspendCurrentTaskFor;

	int prevTop = lua_gettop(mLua);

	lua_getglobal(mLua, cLuaStateHandleKey);
	if (!lua_istable(mLua, -1))
	{
		lua_settop(mLua, prevTop);
		return false;
	}

	lua_getfield(mLua, -1, cInLuaWorkerThreadsTableName);
	if (!lua_istable(mLua, -1))
	{
		lua_settop(mLua, prevTop);
		return false;
	}

	std::size_t nextIdx = lua_objlen(mLua, -1);



	lua_settop(mLua, prevTop);
	return true;
}

//---------------------
// InnerLuaState
// Lua C methods
//---------------------

InnerLuaState* InnerLuaState::l_PopThis(lua_State* pL)
{
	if (!lua_islightuserdata(pL, lua_upvalueindex(1))) return nullptr;
	return (InnerLuaState*)lua_topointer(pL, lua_upvalueindex(1));
}

//------
int InnerLuaState::l_Log(lua_State* pL, LogLevel level)
{
	InnerLuaState* pState = l_PopThis(pL);

	if (pState != nullptr)
	{
		if (!lua_isstring(pL, -1)) return 0;
		pState->mLog.Push(level, lua_tostring(pL, -1));
	}
	return 0;
}

//------
int InnerLuaState::l_LogError(lua_State* pL)
{
	return l_Log(pL, LogLevel::Error);
}

//------
int InnerLuaState::l_LogInfo(lua_State* pL)
{
	return l_Log(pL, LogLevel::Info);
}

//------
int InnerLuaState::l_Sleep(lua_State* pL)
{
	InnerLuaState* pState = l_PopThis(pL);

	if (pState != nullptr)
	{
		if (!lua_isnumber(pL, -1)) return 0;
		lua_Integer millis = lua_tointeger(pL, -1);
		if (millis <= 0) return 0;

		system_clock::time_point sleepTill = system_clock::now() + (millis * 1ms);

		while (system_clock::now() < sleepTill)
		{
			std::unique_lock<std::mutex> lock(pState->mCancelMtx);
			pState->mCancelCv.wait_until(lock, sleepTill);

			if (pState->mCancel) return 0;
		}
	}
	return 0;
}

//------
void InnerLuaState::l_Hook(lua_State* pL, lua_Debug* pDebug)
{
	int stackDelta = -lua_gettop(pL);
	lua_getglobal(pL, cLuaStateHandleKey);
	stackDelta += lua_gettop(pL);

	if (!lua_islightuserdata(pL, -1) || stackDelta == 0)
	{
		lua_pop(pL, stackDelta);
		return;
	}

	InnerLuaState* pState = (InnerLuaState*)lua_topointer(pL, -1);
	lua_pop(pL, 1);

	if (pState != nullptr)
	{
		if (pState->mCancel)
		{
			//Unset hook
			lua_sethook(pL, InnerLuaState::l_Hook, 0,0);
			throw LuaCancellationException();
		}
	}
}

//---------------------
// Public
//---------------------

InnerLuaState::InnerLuaState(LogSection&& log) : mLog(log), mCancel(false), mLua(nullptr)  {}
InnerLuaState::InnerLuaState(const LogSection& log) : mLog(log), mCancel(false), mLua(nullptr) {}

//------
InnerLuaState::~InnerLuaState()
{
	try
	{
		Close();
	}
	catch (const std::exception&)
	{
		//Suppress exceptions in destructor
	}
}

//------
void InnerLuaState::Open()
{
	if (mLua == nullptr && !mCancel) 
	{
		mLua = lua_open();

		luaL_openlibs(mLua);

			lua_createtable(mLua, 0, 2);
				lua_pushlightuserdata(mLua, this);
				lua_pushcclosure(mLua, InnerLuaState::l_LogError, 1);
			lua_setfield(mLua, -2, "LogError");
				lua_pushlightuserdata(mLua, this);
				lua_pushcclosure(mLua, InnerLuaState::l_LogInfo, 1);
			lua_setfield(mLua, -2, "LogInfo");
				lua_pushlightuserdata(mLua, this);
				lua_pushcclosure(mLua, InnerLuaState::l_Sleep, 1);
			lua_setfield(mLua, -2, "Sleep");
		lua_setglobal(mLua, cInLuaWorkerTableName);

			lua_pushlightuserdata(mLua, this);
		lua_setglobal(mLua, cLuaStateHandleKey);

		lua_sethook(mLua, InnerLuaState::l_Hook, LUA_MASKCALL | LUA_MASKCOUNT, (int)1e7);

		mOpen = true;
	}
}

//------
void InnerLuaState::Close()
{
	Cancel(); // Cancel before waiting for any running lua to complete

	if (mLua != nullptr)
	{
		lua_close(mLua);
		mLua = nullptr;
		mOpen = false;
	}
}

//------
void InnerLuaState::ExecTask(std::shared_ptr<Task> task, std::chrono::duration<float> &resumeAfter)
{
	if(mCancel) throw LuaCancellationException();

	if (mLua != nullptr)
	{
		int prevTop = lua_gettop(mLua);

		lua_State* taskThread = lua_newthread(mLua);

		mSuspendCurrentTaskFor = 1000ms; // Default delay in case of a yield
		resumeAfter = -1s;	//By default do not reschedule a task

		task->Exec(taskThread);

		if (task->GetStatus() == TaskStatus::Suspended)
		{
			//TODO
		}
		// If yielded, push to InLuaWorker._Threads
		// and add to recall stack
		// else clear stack

		lua_settop(mLua, prevTop);
	}
}

//------
void InnerLuaState::Cancel()
{
	mCancel = true;

	mCancelCv.notify_all();
}

//------
bool InnerLuaState::IsOpen()
{
	return mOpen;
}