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


#include <chrono>
#include<string>
#include<exception>

#include "CoTask.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using namespace LuaWorker;

CoTask::CoTask(const std::string& funcString, std::unique_ptr<LuaArgBundle> &&argBundle,  TaskConfig&& config) :Task(std::move(config)), mFuncArgs(std::move(argBundle))
{
	mFuncString = "return " + funcString;

}

//-------------------------------
// Protected methods
//-------------------------------

LuaArgBundle CoTask::DoExec(lua_State* pL)
{
	int execResult = luaL_dostring(pL, mFuncString.c_str());

	if (execResult != 0)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error in lua string: " + luaError);
	}

	int argC = 0;

	if (mFuncArgs != nullptr) 
	{
		argC = mFuncArgs->Unpack(pL);
	}

	LuaArgBundle result;

	if (!lua_isfunction(pL, -argC - 1))
	{
		SetError("Invalid coroutine construction.");
		lua_settop(pL, 0);

	}
	else result = DoResume(pL, argC, 1);

	return  result;
}

/// <summary>
/// Resume execution of this task on the given lua state, 
///		passing any arguments already on the stack.
/// If this is not the same state previously passed to Exec, behaviour is undefined.
/// </summary>
/// <param name = "funcCall">1 or 0. 1 Indicates that the function running on this coroutine is on the stack after the arguments</param>
/// <param name="pL">Lua state</param>
LuaArgBundle CoTask::DoResume(lua_State* pL, int argC, int funcCall)
{

	int prevTop = lua_gettop(pL) - argC - funcCall;

	int execResult = lua_resume(pL, argC);	
	
	LuaArgBundle results;

	if (execResult != 0 && execResult != LUA_YIELD)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error resuming task: " + luaError);

	}
	else
	{
		int resC = lua_gettop(pL);// -prevTop;
		results = LuaArgBundle(pL, resC, GetMaxTableDepth());
	}
		
	lua_settop(pL, 0);

	return results;
}

//-------------------------------
// Public methods
//-------------------------------

//------
void CoTask::Exec(lua_State* pL)
{
	if (pL == nullptr || !TrySetRunning()) return;

	LuaArgBundle results = this->DoExec(pL);

	SetResult(std::move(results), lua_status(pL) == LUA_YIELD);

}

//------
void CoTask::Resume(lua_State* pL)
{
	if (pL == nullptr || lua_status(pL) != LUA_YIELD || !TrySetRunning(TaskStatus::Suspended)) return;

	LuaArgBundle results = this->DoResume(pL, 0);
	SetResult(std::move(results), lua_status(pL) == LUA_YIELD);

}
