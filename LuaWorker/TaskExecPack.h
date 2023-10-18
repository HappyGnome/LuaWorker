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

#ifndef _TASK_EXEC_PACK_H_
#define _TASK_EXEC_PACK_H_
#pragma once

#include <memory>

#include "Task.h"
#include "LogSection.h"

namespace LuaWorker
{
	
	class TaskExecPack {
	private:
		
		std::shared_ptr<Task> mTask;

		LogSection mLog;

	public:

		TaskExecPack(const std::shared_ptr<Task>& task, LogSection &&log);

		TaskExecPack(TaskExecPack&&) = default;

		/// <summary>
		/// Not copyable
		/// </summary>
		/// <param name=""></param>
		TaskExecPack(const TaskExecPack&) = delete;

		/// <summary>
		/// Not copyable
		/// </summary>
		/// <param name=""></param>
		TaskExecPack& operator=(const TaskExecPack&) = delete;

		TaskExecPack& operator=(TaskExecPack&&) = default;

		~TaskExecPack();


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
		/// Get execution status of this Task
		/// </summary>
		/// <returns>Current status</returns>
		TaskStatus GetStatus();

		/// <summary>
		/// Permanently cancel execution and any pending tasks
		/// </summary>
		void Cancel();

	};
}

#endif