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

CoTask::CoTask(const std::string& funcString, const std::vector<std::string>& argStrings)
{
	mExecString = "return " + funcString;

	for (const std::string& it : argStrings)
	{
		mExecString += "," + it;
	}
}


//-------------------------------
// Protected methods
//-------------------------------

std::string CoTask::DoExec(lua_State* pL)
{
	int prevTop = lua_gettop(pL);
	int execResult = luaL_dostring(pL, mExecString.c_str());

	if (execResult != 0)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error in lua string: " + luaError);
	}

	std::string ret = "";

	if (lua_gettop(pL) <= prevTop || !lua_isfunction(pL, prevTop + 1))
	{
		SetError("Invalid coroutine construction.");

	}
	else ret = DoResume(pL, lua_gettop(pL) - prevTop - 1);

	lua_settop(pL, prevTop);

	return ret;
}

/// <summary>
/// Resume execution of this task on the given lua state, 
///		passing any 
/// Is this is not the same state previously passed to Exec, behaviour is undefined.
/// </summary>
/// <param name="pL">Lua state</param>
std::string CoTask::DoResume(lua_State* pL, int argC)
{
	int execResult = lua_resume(pL, argC);

	if (execResult != 0 && execResult != LUA_YIELD)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error resuming task: " + luaError);
	}

	std::string ret = "";

	if (lua_type(pL, -1) == LUA_TSTRING) ret = lua_tostring(pL, -1);

	lua_settop(pL, 0);

	return ret;
}

//-------------------------------
// Public methods
//-------------------------------

//------
void CoTask::Exec(lua_State* pL)
{
	if (pL == nullptr || !TrySetRunning()) return;

	std::string res = this->DoExec(pL);

	SetResult(res, lua_status(pL) == LUA_YIELD);

}

//------
void CoTask::Resume(lua_State* pL)
{
	if (pL == nullptr || lua_status(pL) != LUA_YIELD || !TrySetRunning(TaskStatus::Suspended)) return;

	std::string res = this->DoResume(pL, 0);
	SetResult(res , lua_status(pL) == LUA_YIELD);

}
