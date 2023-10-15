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

#ifndef _LUA_TEST_STATE_H_
#define _LUA_TEST_STATE_H_
#pragma once

#include<chrono>

extern "C" {
#include "lua.h"
	//#include "lauxlib.h"
	//#include "lualib.h"
}

using namespace std::chrono_literals;

namespace Tests 
{
	class LuaTestState
	{
	private:
		lua_State* mLua;

		bool GetTestResult();

	public:

		LuaTestState();
		~LuaTestState();

		bool DoTestFile(const char* filepath);
		bool DoTestFile(const char* filepath, std::chrono::duration<float> maxTime, std::chrono::duration<float> minTime = 0ms);
		bool DoTestString(const char* luaString);
		bool DoTestString(const char* luaString, std::chrono::duration<float> maxTime, std::chrono::duration<float> minTime = 0ms);
	};
}

#endif