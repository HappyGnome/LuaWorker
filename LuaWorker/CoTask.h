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
		std::string mExecString;

		/// <summary>
		/// Make initial resume call to start the coroutine
		/// Called on the worker lua state.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Result of the task</returns>
		std::string DoExec(lua_State* pL);

		/// <summary>
		/// Resume execution of the passed thread
		/// </summary>
		/// <param name="pL"></param>
		/// <param name="argC"></param>
		/// <returns></returns>
		std::string DoResume(lua_State* pL, int argC);
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="funcString"></param>
		/// <param name="argStrings"></param>
		explicit CoTask(const std::string& funcString, const std::vector<std::string> &argStrings);

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