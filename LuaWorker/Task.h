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

#ifndef _TASK_H_
#define _TASK_H_
#pragma once

//#include <iostream>
//#include <filesystem>
//#include <thread>
#include <mutex>
#include <string>
#include<variant>

#include "Cancelable.h"
#include "LuaArgBundle.h"
#include "TaskConfig.h"
#include "LuaArgBundle.h"

extern "C" {
#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
}
//using namespace std::chrono_literals;

namespace LuaWorker
{
	enum class TaskStatus {
		NotStarted,
		Running,
		Suspended,
		Complete,	// Final state
		Cancelled,	// Final state
		Error		// Final state
	};

	/// <summary>
	/// Base class for tasks for LuaWorker to execute
	/// </summary>
	class Task : public Cancelable
	{
	private:

		enum TaskType 
		{
			DoSleep,
			DoFile,
			DoString,
			Coroutine
		};

		struct CoroutineData 
		{
			std::string FuncString;
			LuaArgBundle Args;
		};

		//-------------------------------
		// Properties
		//-------------------------------

		TaskStatus mStatus;

		LuaArgBundle mResult;
		std::string mError;

		std::mutex mResultStatusMtx;
		std::condition_variable mResultStatusCv;

		bool mUnreadResult;

		TaskConfig mConfig;

		TaskType mType;

		std::variant<std::string, unsigned int, CoroutineData> mExecData;

		
		//-------------------------------
		// Private methods
		//-------------------------------

		///// <summary>
		///// Explicit constructor with config
		///// </summary>
		//explicit Task(const TaskConfig& config);

		/// <summary>
		/// Explicit constructor with config, hweere exec data is a string
		/// </summary>
		explicit Task(const std::string& data, const TaskConfig& config);

		/// <summary>
		/// Explicit constructor with config, where exec data is uint
		/// </summary>
		explicit Task(unsigned int data, const TaskConfig& config);

		/// <summary>
		/// Explicit constructor with config, where exec data is uint
		/// </summary>
		explicit Task(const std::string& funcString, const LuaArgBundle& args, const TaskConfig& config);

		LuaArgBundle DoExecFile(lua_State* pL);

		LuaArgBundle DoExecSleep(lua_State* pL);

		LuaArgBundle DoExecString(lua_State* pL);

		LuaArgBundle DoExecCoroutine(lua_State* pL);

		LuaArgBundle DoResumeCoroutine(lua_State* pL, int argC, int funcCall);

	protected:

		//-------------------------------
		// Protected methods
		//-------------------------------

		/// <summary>
		/// Set the result of this task. 
		/// </summary>
		/// <param name="results">result bundle</param>
		void SetResult(LuaArgBundle&& results, bool yielded);

		/// <summary>
		/// Try to set the task to a running status.
		/// </summary>
		/// <param name="newResult">False if the current statuis does not support starting the task</param>
		bool TrySetRunning(TaskStatus expected = TaskStatus::NotStarted);

		/// <summary>
		/// Block until specified time has elapsed (or task cancelled)
		/// </summary>
		/// <param name="waitForMillis">Min time to wait</param>
		void SleepFor(unsigned int waitForMillis);

	public:

		//------------------------------
		// Diagnostic statics
		//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		static std::atomic<int> CountPushed;
		static std::atomic<int> CountDeleted;
		static std::atomic<int> PeakTaskCount ;
#endif


		//-------------------------------
		// Public static methods
		//-------------------------------

		/// <summary>
		/// Check if status is final
		/// </summary>
		/// <returns>True if status is final</returns>
		static bool IsFinal(TaskStatus status);

		/// <summary>
		/// Construct instance to execute a lua file
		/// </summary>
		/// <param name="filePath">Filepath to run when this task executes</param>
		static std::shared_ptr<Task> NewDoFile(const std::string& filePath, const TaskConfig& config);

		/// <summary>
		/// Construct instance to execute a lua string
		/// </summary>
		static std::shared_ptr<Task> NewDoString(const std::string& luaString, const TaskConfig& config);

		/// <summary>
		/// Construct instance that sleeps the worker thread
		/// </summary>
		/// <param name="sleepForMs"></param>
		/// <param name="config"></param>
		/// <returns></returns>
		static std::shared_ptr<Task> NewDoSleep(unsigned int sleepForMs, const TaskConfig& config);

		/// <summary>
		/// Construct instance that launches a coroutine
		/// </summary>
		/// <param name="funcString"></param>
		/// <param name="args"></param>
		/// <param name="config"></param>
		/// <returns></returns>
		static std::shared_ptr<Task> NewDoCoroutine(const std::string& funcString, const LuaArgBundle& args, const TaskConfig& config);

		//-------------------------------
		// Public methods
		//-------------------------------

#ifdef _BENCHMARK_OBJ_COUNTERS_
		~Task()
#endif

		Task() = default;

		Task(Task&&) noexcept = delete;
		Task& operator= (Task&&) = delete;

		Task(const Task&) = delete;
		Task& operator= (const Task&) = delete;

		/// <summary>
		/// Push result of this task onto the given lua stack and return the number of items pushed
		/// </summary>
		/// <returns>The number of items added to the stack</returns>
		int GetResult(lua_State *pL);

		/// <summary>
		/// Block until task has executed (or reaches a final state) 
		/// </summary>
		/// <param name="waitForMillis">Max time to wait</param>
		/// <returns>True if tasks is complete, or newly suspended</returns>
		bool WaitForResult(long waitForMillis);

		/// <summary>
		/// Get execution status of this Task
		/// </summary>
		/// <returns>Current status</returns>
		TaskStatus GetStatus();


		/// <summary>
		/// Permanently cancel execution and any pending tasks
		/// </summary>
		void Cancel();

		/// <summary>
		/// Set an error status for this task
		/// </summary>
		/// <param name="errorMessage">Error message to set</param>
		void SetError(const std::string& errorMessage);

		/// <summary>
		/// Get current error message
		/// </summary>
		/// <returns>Error message</returns>
		std::string GetError();

		/// <summary>
		/// Get the MaxTableDepth option for this task. This controls the number of levels of nesting in tables that can be passed to or returned from this Task.
		/// E.g. if it's 0, no tables can be returned. If 1, then tables in result values will not contain tables
		/// </summary>
		/// <returns></returns>
		int GetMaxTableDepth();

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

		/// <summary>
		/// Returns true if this task should be run like a coroutine on its own lua thread
		/// </summary>
		/// <returns></returns>
		bool RunOnOwnLuaThread() const;

	};
}
#endif