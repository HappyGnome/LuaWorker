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

#ifndef _TASK_H_
#define _TASK_H_

//#include <iostream>
//#include <filesystem>
//#include <thread>
#include <mutex>
#include <string>

#include "Cancelable.h"

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

		//-------------------------------
		// Properties
		//-------------------------------

		TaskStatus mStatus;

		std::string mResult;
		std::string mError;

		std::mutex mResultStatusMtx;
		std::condition_variable mResultStatusCv;

		//-------------------------------
		// Private methods
		//-------------------------------

		/// <summary>
		/// Set the result of this task
		/// </summary>
		/// <param name="newResult">Value of the result to set</param>
		void SetResult(const std::string& newResult);



	protected:

		//-------------------------------
		// Protected methods
		//-------------------------------

		/// <summary>
		/// Do the work for this task, as implemented in derived classes.
		/// Called on the worker lua state.
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Result of the task</returns>
		virtual std::string DoExec(lua_State* pL) = 0;

		/// <summary>
		/// Resume execution of this task on the given lua state, 
		///		passing any 
		/// Is this is not the same state previously passed to Exec, behaviour is undefined.
		/// </summary>
		/// <param name="pL">Lua state</param>
		virtual std::string DoResume(lua_State* pL);

		/// <summary>
		/// Block until specified time has elapsed (or task cancelled)
		/// </summary>
		/// <param name="waitForMillis">Min time to wait</param>
		void SleepFor(unsigned int waitForMillis);

	public:

		//-------------------------------
		// Public static methods
		//-------------------------------

		/// <summary>
		/// Check if status is final
		/// </summary>
		/// <returns>True if status is final</returns>
		static bool IsFinal(TaskStatus status);

		//-------------------------------
		// Public methods
		//-------------------------------

		/// <summary>
		/// Default constructor
		/// </summary>
		Task();

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
		/// Get result of this task
		/// </summary>
		/// <returns>Task result</returns>
		std::string GetResult();

		/// <summary>
		/// Block until task has executed (or reaches a final state) 
		/// </summary>
		/// <param name="waitForMillis">Max time to wait</param>
		void WaitForResult(unsigned int waitForMillis);

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

	};
}
#endif