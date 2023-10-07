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
#include <string>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace fs = std::filesystem;


void DoFile(std::filesystem::path  path)
{
	lua_State* pL = lua_open();

	try
	{
		luaL_openlibs(pL);

		int execResult = luaL_dofile(pL, path.string().c_str());

		if (execResult != 0) {
			const char* err = lua_tostring(pL, -1);
			std::cout << err << std::endl;
		}

	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	lua_close(pL);
}

int main()
{
	fs::path here = fs::current_path();
	here += "\\LuaExamples\\";
	for (auto const& entry : fs::directory_iterator(here))
	{
		if (!fs::is_regular_file(entry)) continue;

		fs::path path = entry.path();

		if (path.extension().string() != ".lua") continue;

		std::cout << "Running " << path.filename() << std::endl;

		DoFile(path);
	}

}