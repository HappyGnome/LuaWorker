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

using namespace LuaWorker;

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
void InnerLuaState::l_Hook(lua_State* pL, lua_Debug* pDebug)
{
	InnerLuaState* pState = l_PopThis(pL);

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

//------
InnerLuaState::~InnerLuaState()
{
	try
	{
		Close();
	}
	catch (const std::exception& ex)
	{
		//Suppress exceptions in destructor
	}
}

//------
void InnerLuaState::Open()
{
	std::unique_lock<std::mutex> lock(mLuaMtx);

	mCancel = false;

	if (mLua == nullptr) 
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
		lua_setglobal(mLua, "LuaWorker");

		lua_sethook(mLua, InnerLuaState::l_Hook, LUA_HOOKCALL | LUA_HOOKCOUNT, 10000);

		mOpen = true;
	}
}

//------
void InnerLuaState::Close()
{
	Cancel(); // Cancel before waiting for any running lua to complete

	std::unique_lock<std::mutex> lock(mLuaMtx);

	if (mLua != nullptr)
	{
		lua_close(mLua);
		mLua = nullptr;
		mOpen = false;
	}
}

//------
void InnerLuaState::ExecTask(std::shared_ptr<Task> task)
{
	std::unique_lock<std::mutex> lock(mLuaMtx);

	if(mCancel) throw LuaCancellationException();

	if (mLua != nullptr)
	{
		task->Exec(mLua);
	}
}

//------
void InnerLuaState::Cancel()
{
	mCancel = true;
}

//------
bool InnerLuaState::IsOpen()
{
	return mOpen;
}