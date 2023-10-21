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
//#include "TaskPackAcceptor.h"
#include "InnerLuaState.h"

namespace LuaWorker
{
	class TaskPackAcceptor;

	class TaskExecPack
	{
	protected:

		/// <summary>
		/// Pass this instance to appropriate acceptor of a given LuaState using move semantics.
		/// May invalidate the lvalue instance used to call it
		/// </summary>
		/// <param name="pLua"></param>
		virtual void CastAndExec(std::unique_ptr<TaskExecPack>&& thisAsBase, InnerLuaState* pLua) = 0;

	public:

		/// <summary>
		/// Pass ExecPack to appropriate handler in an TaskPackAcceptor
		/// </summary>
		/// <param name="visitor"></param>
		static void VisitLuaState(std::unique_ptr<TaskExecPack>&& visitor, InnerLuaState* pLua);

		/// <summary>
		/// Default constructor
		/// </summary>
		TaskExecPack() = default;

		/// <summary>
		/// Move constructor
		/// </summary>
		/// <param name=""></param>
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

		/// <summary>
		/// Move assignment
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		TaskExecPack& operator=(TaskExecPack&&) = default;

		/// <summary>
		/// Destructor
		/// </summary>
		~TaskExecPack() = default;

		/// <summary>
		/// Get execution status of this Task
		/// </summary>
		/// <returns>Current status</returns>
		virtual TaskStatus GetStatus() = 0;

		/// <summary>
		/// Permanently cancel execution and any pending tasks
		/// </summary>
		virtual void Cancel() = 0;

	};

}
#endif