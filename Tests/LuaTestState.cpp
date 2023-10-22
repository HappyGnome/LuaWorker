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

#include<chrono>
#include<filesystem>

#include "LuaTestState.h"
#include "CppUnitTest.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Tests;
using namespace std::chrono_literals;
using std::chrono::system_clock;


//--------------------------
// Private
//--------------------------
bool LuaTestState::GetTestResult()
{
	if (mLua == nullptr) return false;

	if (!lua_isboolean(mLua, -1)) return false;

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

	std::filesystem::path here = std::filesystem::current_path().append("..\\..\\Tests\\");
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

	std::filesystem::path path = std::filesystem::current_path().append("..\\..\\Tests\\");
	path += "\\LuaTests\\";
	path += filepath;

	int execResult = luaL_dofile(mLua, path.string().c_str());

	if (execResult != 0) {
		const char* err = lua_tostring(mLua, -1);
		Logger::WriteMessage(err);
		return false;
	}

	return LuaTestState::GetTestResult();
}

bool LuaTestState::DoTestFile(const char* filepath, std::chrono::duration<float> maxTime, std::chrono::duration<float> minTime)
{
	system_clock::time_point start = system_clock::now();

	if (!DoTestFile(filepath)) return false;

	std::chrono::duration<float> elapsed = system_clock::now() - start;

	if (elapsed < minTime)
	{
		Logger::WriteMessage("Min time not reached");
		return false;
	}
	else if (elapsed > maxTime)
	{
		Logger::WriteMessage("Max time exceeded");
		return false;
	}
	else return true;
}

bool LuaTestState::DoTestString(const char* luaString)
{
	if (mLua == nullptr) return false;

	int execResult = luaL_dostring(mLua, luaString);

	if (execResult != 0) {
		const char* err = lua_tostring(mLua, -1);
		Logger::WriteMessage(err);
		return false;
	}

	return LuaTestState::GetTestResult();
}

bool LuaTestState::DoTestString(const char* luaString, std::chrono::duration<float> maxTime, std::chrono::duration<float> minTime)
{
	system_clock::time_point start = system_clock::now();

	if (!DoTestString(luaString)) return false;

	std::chrono::duration<float> elapsed = system_clock::now() - start;

	if (elapsed < minTime)
	{
		Logger::WriteMessage("Min time not reached");
		return false;
	}
	else if (elapsed > maxTime)
	{
		Logger::WriteMessage("Max time exceeded");
		return false;
	}
	else return true;
}

