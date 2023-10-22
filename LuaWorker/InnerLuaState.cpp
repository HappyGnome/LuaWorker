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

//#include "TaskExecPack.h"
#include "OneShotTaskExecPack.h"
#include "CoTaskExecPack.h"

using namespace std::chrono_literals;
using std::chrono::system_clock;

using namespace LuaWorker;

const char* InnerLuaState::cLuaRegistryThisKey = "LUAWORKER_STATE_THIS";
const char* InnerLuaState::cLuaRegistryThreadTableKey = "LUAWORKER_STATE_THREADS";
const char* InnerLuaState::cInLuaWorkerTableName = "InLuaWorker";

//----------------------
// Private
//----------------------

bool InnerLuaState::HandleSuspendedTask(std::unique_ptr<CoTaskExecPack>&& task, std::chrono::system_clock::time_point resumeAt)
{
	if (mLua == nullptr) return false;

	if (!lua_isthread(mLua, -1)) return false;

	int prevTop = lua_gettop(mLua);

	lua_pushlightuserdata(mLua, &cLuaRegistryThreadTableKey);
	lua_gettable(mLua, LUA_REGISTRYINDEX);

	if (!lua_istable(mLua, -1))
	{
		lua_settop(mLua, prevTop - 1);
		return false;
	}

	T_SuspendedTaskCard card  = mResumableTasks.MakeCard(std::move(task),resumeAt);

	lua_pushinteger(mLua, card.GetTag());

	lua_insert(mLua, -3); // handle (key) below thread
	lua_insert(mLua, -3); // Threads table below handle

	lua_settable(mLua, -3); // Add thread to threads table at card tag index

	T_SuspendedTaskCard::Return(std::move(card));

	lua_settop(mLua, prevTop);
	return true;
}

lua_State* InnerLuaState::GetTaskThread(int taskHandle)
{
	if (mLua == nullptr) return nullptr;

	int prevTop = lua_gettop(mLua);

	lua_pushlightuserdata(mLua, &cLuaRegistryThreadTableKey);
	lua_gettable(mLua, LUA_REGISTRYINDEX);

	if (!lua_istable(mLua, -1)) {
		lua_settop(mLua, prevTop);
		return nullptr;
	}

	lua_pushinteger(mLua, taskHandle);
	lua_gettable(mLua, -2);

	if (!lua_isthread(mLua, -1))
	{
		lua_settop(mLua, prevTop);
		return nullptr;
	}

	lua_State* taskThread = lua_tothread(mLua, -1);
	lua_settop(mLua, prevTop);

	return taskThread;
}

void InnerLuaState::RemoveTaskThread(int taskHandle)
{
	if (mLua == nullptr) return;

	int prevTop = lua_gettop(mLua);

	lua_pushlightuserdata(mLua, &cLuaRegistryThreadTableKey);
	lua_gettable(mLua, LUA_REGISTRYINDEX);

	if (!lua_istable(mLua, -1)) {
		lua_settop(mLua, prevTop);
		return;
	}

	lua_pushinteger(mLua, taskHandle);
	lua_pushnil(mLua);
	lua_gettable(mLua, -3);
	lua_settop(mLua, prevTop);
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

int InnerLuaState::l_YieldFor(lua_State* pL)
{
	InnerLuaState* pState = l_PopThis(pL);

	if (pState != nullptr)
	{
		int argC = lua_gettop(pL);

		if (!pState->mCurrentTaskCanYield)
		{
			lua_pushstring(pL, "Cannot yield here.");
			lua_error(pL);
			return 0;
		}

		if (!lua_isnumber(pL, -argC)) {
			lua_pushstring(pL, "YieldFor expects parameters (<number>,...<results>)");
			lua_error(pL);
			return 0;
		}
		lua_Integer millis = lua_tointeger(pL, -argC);
		if (millis <= 0)
		{
			lua_pushstring(pL, "YieldFor delay must be positive");
			lua_error(pL);
			return 0;
		}

		pState -> mResumeCurrentTaskAt = system_clock::now() + (millis * 1ms);
		pState -> mCurrentTaskYielded = true;

		int resultCount = 0;
		if (argC > 1 && lua_isstring(pL, 2))
		{
			lua_settop(pL, 2);
			resultCount = 1;
		}

		return lua_yield(pL, resultCount); //Yield remaining parameters to resume
	}
	return 0;
}

//------
void InnerLuaState::l_Hook(lua_State* pL, lua_Debug* pDebug)
{
	int prevTop = lua_gettop(pL);
	lua_pushlightuserdata(pL, &cLuaRegistryThisKey);
	lua_gettable(pL, LUA_REGISTRYINDEX);

	if (!lua_islightuserdata(pL, -1))
	{
		lua_settop(pL, prevTop);
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

InnerLuaState::InnerLuaState(LogSection&& log) 
	: mLog(log), 
	mCancel(false), 
	mLua(nullptr), 
	mResumableTasks(),
	mResumeCurrentTaskAt(),
	mCurrentTaskYielded(),
	mCurrentTaskCanYield(){}
InnerLuaState::InnerLuaState(const LogSection& log) 
	: mLog(log), 
	mCancel(false), 
	mLua(nullptr), 
	mResumableTasks(),
	mResumeCurrentTaskAt(),
	mCurrentTaskYielded(),
	mCurrentTaskCanYield(){}

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

		// InLuaWorker object
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

		lua_pushlightuserdata(mLua, this);
		lua_pushcclosure(mLua, InnerLuaState::l_YieldFor, 1);
		lua_setfield(mLua, -2, "YieldFor");

		lua_setglobal(mLua, cInLuaWorkerTableName);

		// This pointer in registry
		lua_pushlightuserdata(mLua, &cLuaRegistryThisKey);
		lua_pushlightuserdata(mLua, this);
		lua_settable(mLua, LUA_REGISTRYINDEX);

		// Threads table in registry
		lua_pushlightuserdata(mLua, &cLuaRegistryThreadTableKey);
		lua_createtable(mLua, 0, 0);
		lua_settable(mLua, LUA_REGISTRYINDEX);

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
void InnerLuaState::ExecTask(std::unique_ptr<OneShotTaskExecPack>&& task)
{
	if(mCancel) throw LuaCancellationException();

	if (mLua != nullptr && task != nullptr)
	{
		int prevTop = lua_gettop(mLua);

		mCurrentTaskCanYield = false; // Block yields via InLuaWorker

		task->Exec(mLua);

		lua_settop(mLua, prevTop);
	}
}

void InnerLuaState::ExecTask(std::unique_ptr < CoTaskExecPack>&& task)
{
	if (mCancel) throw LuaCancellationException();

	if (mLua != nullptr && task != nullptr)
	{
		int prevTop = lua_gettop(mLua);

		lua_State* taskThread = lua_newthread(mLua);

		mCurrentTaskYielded = false;
		mCurrentTaskCanYield = true;

		task->Exec(taskThread);

		if (mCurrentTaskYielded && lua_status(taskThread) == LUA_YIELD)
		{
			HandleSuspendedTask(std::move(task), mResumeCurrentTaskAt);
		}

		lua_settop(mLua, prevTop);
	}
}

void InnerLuaState::ResumeTask()
{
	if (mCancel) throw LuaCancellationException();

	std::optional<T_SuspendedTaskCard> card(mResumableTasks.PopIfLess(std::chrono::system_clock::now()));

	if (!card.has_value()) return;

	lua_State* taskThread = GetTaskThread(card.value().GetTag());

	if (taskThread == nullptr) return;

	mCurrentTaskYielded = false;

	card.value().GetValue()->Resume(taskThread);

	if (mCurrentTaskYielded)
	{
		card.value().SetSortKey(mResumeCurrentTaskAt);
		T_SuspendedTaskCard::Return(std::move(card.value()));
	}
	else
	{
		RemoveTaskThread(card.value().GetTag());
	}
}


//------
std::optional<std::chrono::system_clock::time_point> InnerLuaState::GetNextResume()
{
	return mResumableTasks.GetThreshold();
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