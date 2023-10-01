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

#include <iostream>
#include <filesystem>
#include <thread>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
using namespace std::chrono_literals;
int main()
{
	lua_State* pL = lua_open();
	luaL_openlibs(pL);

	std::filesystem::path path = std::filesystem::current_path().parent_path();
	path+="\\LuaTest\\run_worker.lua";

	std::filesystem::path binpath = std::filesystem::current_path().parent_path().parent_path().parent_path();

	lua_pushfstring(pL, binpath.string().c_str());
	lua_setglobal(pL, "BinDir");

	if (luaL_dofile(pL,  path.string().c_str()) == 0) {
		std::cout << "Lua file run" << std::endl;
	}
	else { std::cout << lua_tostring(pL, -1) << std::endl; }
	lua_close(pL);
	return 0;
}
