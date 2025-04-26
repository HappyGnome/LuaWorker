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

#ifndef _SINGLE_TASK_H_
#define _SINGLE_TASK_H_
#pragma once

#include <string>

#include "Cancelable.h"
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
	/// Base class for simple (non-coroutine-based) tasks for LuaWorker to execute
	/// </summary>
	class OneShotTask : public Task
	{
	protected:

		//-------------------------------
		// Protected methods
		//-------------------------------

		/// <summary>
		/// Do the work for this task, as implemented in derived classes.
		/// Called on the worker lua state.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Bundled results</returns>
		virtual LuaArgBundle DoExec(lua_State* pL) = 0;

	public:

		/// <summary>
		/// Execute this task on a given lua state
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Exec(lua_State* pL);

	};
}
#endif