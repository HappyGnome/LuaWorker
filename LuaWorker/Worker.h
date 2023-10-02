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

#ifndef _WORKER_H_
#define _WORKER_H_

#include <iostream>
#include <filesystem>
#include <thread>
#include <deque> 
#include <mutex> 

#include "Task.h"
#include "LogSection.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
using namespace std::chrono_literals;

namespace LuaWorker
{
	enum class WorkerStatus {
		NotStarted,
		Starting,
		Processing,
		Cancelled,	// Final
		Error		// Final
	};

	/// <summary>
	/// Class to manage a worker thread executing Tasks in a lua instance
	/// </summary>
	class Worker
	{
	private:

		std::thread mThread;

		std::deque<std::shared_ptr<Task>> mTaskQueue;
		std::mutex mTasksMtx;

		std::condition_variable mTaskCancelCv;

		std::atomic<bool> mCancel;

		std::atomic<WorkerStatus> mCurrentStatus;

		LogSection mLog;

		//---------------------
		// Private Methods
		//---------------------

		/// <summary>
		/// Method executed by the worker thread
		/// </summary>
		void ThreadMain();
	
		/// <summary>
		/// Setup lua environment for main thread
		/// </summary>
		/// <returns>Lua state created, or nullptr</returns>
		lua_State* ThreadMainInitLua();

		/// <summary>
		/// Execute main worker loop
		/// </summary>
		/// <param name="pL">Lua state in which to execute tasks</param>
		void ThreadMainLoop(lua_State* pL);

		/// <summary>
		/// Cancel worker thread execution
		/// </summary>
		void Cancel();

		/// <summary>
		/// Set all pending tasks to a Cancelled state
		/// </summary>
		void CancelAllTasks();

		//---------------------
		// Worker Lua 
		// C methods
		//---------------------
		static Worker* l_PopThis(lua_State* pL);
		static int l_Log (lua_State* pL, LogLevel level);
		static int l_LogError (lua_State* pL);
		static int l_LogInfo (lua_State* pL);

	public:

		//---------------------
		// Public Methods
		//---------------------

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="log">Log to push errors and messages to</param>
		Worker(LogSection &&log);

		/// <summary>
		/// Destructor
		/// </summary>
		~Worker();

		/// <summary>
		/// Start worker thread
		/// </summary>
		/// <returns>Current worker status</returns>
		WorkerStatus Start();

		/// <summary>
		/// Stop worker thread (blocking)
		/// </summary>
		/// <returns>Current worker status</returns>
		WorkerStatus Stop();

		/// <summary>
		/// Get current worker thread status
		/// </summary>
		/// <returns>Current worker status</returns>
		WorkerStatus GetStatus();

		/// <summary>
		/// Queue task for worker thread
		/// </summary>
		/// <param name="task">Task to queue</param>
		/// <returns>Current worker status</returns>
		WorkerStatus AddTask(std::shared_ptr<Task> task);

	};
}
#endif