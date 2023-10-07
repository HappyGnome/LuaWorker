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
 
#pragma once

#ifndef _WORKER_LUA_INTERFACE_H_
#define _WORKER_LUA_INTERFACE_H_

//#include <iostream>
//#include <filesystem>
//#include <thread>

#include <memory>

#include "AutoKeyMap.h"
#include "LogStack.h"
#include "Worker.h"
#include "TaskLuaInterface.h"

extern "C" {
#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
}
//using namespace std::chrono_literals;

namespace LuaWorker
{
	/// <summary>
	/// Lua interface for the Worker objects
	/// </summary>
	class WorkerLuaInterface
	{
	private:

		static std::shared_ptr<LogStack> sLog;
		static AutoKeyMap<int, Worker> sWorkers;
		static int sNextWorkerId;

		//-------------------------------
		// Static Lua helper methods
		//-------------------------------

		/// <summary>
		/// Pop Worker handle from the top of the lua stack and return
		/// the shared_ptr to the corresponding Worker.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Worker (if found), or nullptr</returns>
		static std::shared_ptr<Worker> l_PopWorker(lua_State* pL);

		/// <summary>
		/// Pop Worker handle from the top of the lua stack and return
		/// the shared_ptr to the corresponding Worker.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <param name="outKey">Key of the Worker found in mWorkers</param>
		/// <returns>Worker (if found), or nullptr</returns>
		static std::shared_ptr<Worker> l_PopWorker(lua_State* pL, int& outKey);

		/// <summary>
		/// Push status of a Worker to the top of the lua stack
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <param name="p">Worker whose state to push</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_PushStatus(lua_State* pL, std::shared_ptr<Worker> pWorker);

	public:

		//-------------------------------
		// Lua Constants
		//-------------------------------
		static const int
			WorkerStatus_NotStarted = 0,
			WorkerStatus_Starting = 1,
			WorkerStatus_Processing = 2,
			WorkerStatus_Cancelled = 3,
			WorkerStatus_Error = 4;

		static const int
			LogLevel_Info = 0,
			LogLevel_Warn = 1,
			LogLevel_Error = 2;

		//-------------------------------
		// Static Lua-callable methods 
		// (Library level)
		//-------------------------------

		/// <summary>
		/// Create a new worker instance
		/// 
		/// Lua syntax:
		///		local worker = LuaWorker.Create()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_Create(lua_State* pL);

		/// <summary>
		/// Pop one log entry from library level logger
		/// 
		/// Lua syntax:
		///		local logLine, level = LuaWorker.PopLogLine()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_PopLogLine(lua_State* pL);

		//-------------------------------
		// Static Lua-callable methods 
		// (Worker Object)
		//-------------------------------

		/// <summary>
		/// Pop Worker handle from the top of the lua stack. 
		/// If it's valid, release that worker instance.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_Delete(lua_State* pL);

		/// <summary>
		/// Add executable string task to the worker queue.
		/// 
		/// Lua syntax:
		///		local task = worker:DoString("os.execute('timeout 5')")
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_DoString(lua_State* pL);

		/// <summary>
		/// Add executable file task to the worker queue.
		/// 
		/// Lua syntax:
		///		local task = worker:DoFile("myfile.lua")
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_DoFile(lua_State* pL);

		/// <summary>
		/// Add thread sleep action to the worker queue.
		/// 
		/// Lua syntax:
		///		local task = worker:DoSleep(1000) -- milliseconds
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_DoSleep(lua_State* pL);

		/// <summary>
		/// Start worker thread
		/// 
		/// Lua syntax:
		///		local status = worker:Start()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_Start(lua_State* pL);

		/// <summary>
		/// Stop worker thread and block until execution completes 
		/// (cancels pending tasks)
		/// 
		/// Lua syntax:
		///		local status = worker:Stop()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_Stop(lua_State* pL);

		/// <summary>
		/// Get current status of a worker
		/// 
		/// Lua syntax:
		///		local status = worker:Status()
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns>Number of items pushed to the stack</returns>
		static int l_Worker_Status(lua_State* pL);
	};
}
#endif