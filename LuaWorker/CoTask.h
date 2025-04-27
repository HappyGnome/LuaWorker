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

#ifndef _COTASK_H_
#define _COTASK_H_
#pragma once

//#include <iostream>
//#include <filesystem>
//#include <thread>
#include <vector>
#include <string>

#include "Task.h"
#include "LuaArgBundle.h"

extern "C" {
#include "lua.h"
	//#include "lauxlib.h"
	//#include "lualib.h"
}
//using namespace std::chrono_literals;

namespace LuaWorker
{
	/// <summary>
	/// Task to start a coroutine on the worker thread
	/// </summary>
	class CoTask : public Task
	{
	private:
		std::string mFuncString;

		std::unique_ptr<LuaArgBundle> mFuncArgs;

		/// <summary>
		/// Make initial resume call to start the coroutine
		/// Called on the worker lua state.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>THe number of result values on the lua stack</returns>
		LuaArgBundle DoExec(lua_State* pL);

		/// <summary>
		/// Resume execution of the passed thread
		/// </summary>
		/// <param name="pL"></param>
		/// <param name="argC"></param>
		/// <param name = "funcCall">1 or 0. 1 Indicates that the function running on this coroutine is on the stack after the arguments</param>
		/// <returns>Number of result values on the lua stack</returns>
		LuaArgBundle DoResume(lua_State* pL, int argC, int funcCall = 0);
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="funcString"></param>
		/// <param name="argStrings"></param>
		explicit CoTask(const std::string& funcString, std::unique_ptr<LuaArgBundle> &&argBundle, TaskConfig&& config);

		/// <summary>
		/// Execute this task on a given lua state
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Exec(lua_State* pL);

		/// <summary>
		/// Resume execution of this task on the given lua state.
		/// Is this is not the same state previously passed to Exec, behaviour is undefined.
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Resume(lua_State* pL);

	};
}
#endif