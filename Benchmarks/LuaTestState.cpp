
/*****************************************************************************\
*
*  Copyright 2025 HappyGnome
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

#include<chrono>
#include<filesystem>
#include<iostream>

#include "LuaTestState.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


using namespace std::chrono_literals;
using std::chrono::system_clock;

namespace Benchmarks
{
	//--------------------------
	// Private
	//--------------------------

	bool LuaTestState::GetTestResult(std::string &details)
	{
		if (mLua == nullptr) return false;

		if (!lua_isboolean(mLua, -1)) return false;
		if (lua_isstring(mLua, -2))
		{
			details = lua_tostring(mLua, -2);
		}
		else
		{
			details = "";
		}

		bool ret = lua_toboolean(mLua, -1);

		lua_settop(mLua, 0);

		return ret;
	}



	//--------------------------
	// Public
	//--------------------------
	LuaTestState::LuaTestState()
	{
		mLua = lua_open();
		luaL_openlibs(mLua);

		std::filesystem::path here = std::filesystem::current_path().append("..\\Tests\\");
		here += "\\LuaTests\\";

		lua_pushfstring(mLua, here.string().c_str());
		lua_setglobal(mLua, "RootDir");

	}

	LuaTestState::~LuaTestState()
	{
		if (mLua == nullptr) return;

		lua_close(mLua);
		mLua = nullptr;

	}

	bool LuaTestState::DoTestFile(const char* filepath)
	{
		if (mLua == nullptr) return false;

		std::filesystem::path path = std::filesystem::current_path().append("..\\Tests\\");
		path += "LuaTests\\";
		path += filepath;

		int execResult = luaL_dofile(mLua, path.string().c_str());

		if (execResult != 0) {
			const char* err = lua_tostring(mLua, -1);
			std::cout << (err) << std::endl;
			return false;
		}

		std::string details;
		bool ok = LuaTestState::GetTestResult(details);

		if (details != "") 
		{
			std::cout << details<< std::endl;
		}

		return ok;
	}

	bool LuaTestState::DoTestString(const char* luaString)
	{
		if (mLua == nullptr) return false;

		int execResult = luaL_dostring(mLua, luaString);

		if (execResult != 0) {
			const char* err = lua_tostring(mLua, -1);
			std::cout << (err) << std::endl;
			return false;
		}

		std::string details;
		bool ok = LuaTestState::GetTestResult(details);

		if (details != "") 
		{
			std::cout << details << std::endl;
		}

		return ok;
	}

	void LuaTestState::RunGcNow()
	{
		if (mLua == nullptr) return;

		lua_gc(mLua, LUA_GCCOLLECT, 0);
	}
};
