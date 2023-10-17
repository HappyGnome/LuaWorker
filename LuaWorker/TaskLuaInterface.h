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

#ifndef _TASK_LUA_INTERFACE_H_
#define _TASK_LUA_INTERFACE_H_
#pragma once

//#include <iostream>
//#include <filesystem>
//#include <thread>
//#include <mutex>
//#include <string>
//#include <vector>
//#include <map>
#include<memory>

#include "AutoKeyMap.h"
#include "Task.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
//using namespace std::chrono_literals;

namespace LuaWorker
{
	/// <summary>
	/// Lua interface for the Task objects
	/// </summary>
	class TaskLuaInterface
	{
	private:

		static AutoKeyDeck::AutoKeyMap<int, Task> sTasks;

		//-------------------------------
		// Static Lua helper methods
		//-------------------------------
		static std::shared_ptr<Task> l_PopTask(lua_State* pL);
		static std::shared_ptr<Task> l_PopTask(lua_State* pL, int& outKey);

	public:

		//-------------------------------
		// Lua Constants
		//-------------------------------
		static const int
			TaskStatus_NotStarted	= 0,
			TaskStatus_Running		= 1,
			TaskStatus_Complete		= 2,
			TaskStatus_Cancelled	= 3,
			TaskStatus_Error		= 4;

		/// <summary>
		/// Push task handle to the top of the lua stack and keep shared_ptr
		/// to the task internally
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <param name="pTask">Task object to store and push</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_PushTask(lua_State* pL, std::shared_ptr<Task> pTask);

		//-------------------------------
		// Static Lua-callable methods
		//-------------------------------

		/// <summary>
		/// Delete Task object at the top of the lua stack
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Task_Delete(lua_State* pL);

		/// <summary>
		/// Block until task at the top of the lua stack 
		/// completes. If the task returned a result, leave
		/// this at the top of the lua stack.
		/// 
		/// Lua syntax:
		///		local resultString = task:Await()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Task_Await(lua_State* pL);

		/// <summary>
		/// Pop Task handle from the top of the lua stack
		/// return the TaskStatus_... constant representing
		/// the task's state.
		/// 
		/// Lua syntax:
		///		local status = task:Status()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Task_Status(lua_State* pL);

		/// <summary>
		/// Pop Task handle from the top of the lua stack
		/// return true if tasks is in a final state
		/// 
		/// Lua syntax:
		///		local final = task:Finalized()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Task_Finalized(lua_State* pL);
	};
};
#endif