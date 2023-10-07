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

#include "TaskDoString.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	//#include "lualib.h"
}

using namespace LuaWorker;

TaskDoString::TaskDoString(std::string execString) : mExecString(execString) {}

std::string TaskDoString::DoExec(lua_State* pL)
{
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

	if (lua_type(pL, -1) == LUA_TSTRING) ret = lua_tostring(pL, -1);

	lua_settop(pL, 0);

	return ret;
}


