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

#ifndef _TYPED_TASK_EXEC_PACK_H_
#define _TYPED_TASK_EXEC_PACK_H_
#pragma once

#include <memory>

#include "Task.h"
#include "TaskExecPack.h"
#include "LogSection.h"

namespace LuaWorker
{

	template<typename T_Task, class T_Class>
	class TypedTaskExecPack : public TaskExecPack
	{
	protected:

		virtual void CastAndExec(std::unique_ptr<TaskExecPack>&& thisAsBase, InnerLuaState* pLua) final
		{
			if (pLua == nullptr || thisAsBase.get() != this) return;

			std::unique_ptr<T_Class> cast((T_Class*)thisAsBase.get());
			thisAsBase.release();

			pLua->ExecTask(std::move(cast));
		}


		std::shared_ptr<T_Task> mTask;

		LogSection mLog;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="task"></param>
		/// <param name="log"></param>
		explicit TypedTaskExecPack(const std::shared_ptr<T_Task>& task, LogSection&& log) :
			mTask(task), mLog(log) {}

		/// <summary>
		/// Move constructor
		/// </summary>
		/// <param name=""></param>
		TypedTaskExecPack(TypedTaskExecPack&&) = default;

		/// <summary>
		/// Not copyable
		/// </summary>
		/// <param name=""></param>
		TypedTaskExecPack(const TypedTaskExecPack&) = delete;

		/// <summary>
		/// Not copyable
		/// </summary>
		/// <param name=""></param>
		TypedTaskExecPack& operator=(const TypedTaskExecPack&) = delete;

		/// <summary>
		/// Move assignment
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		TypedTaskExecPack& operator=(TypedTaskExecPack&&) = default;

		/// <summary>
		/// Destructor
		/// </summary>
		~TypedTaskExecPack()
		{
			if (mTask == nullptr) return;

			try
			{
				mTask->Cancel();
			}
			catch (const std::exception&)
			{
				// Suppress exception in destructor
			}
		}


		/*/// <summary>
		/// Execute this task on a given lua state
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Exec(lua_State* pL);

		/// <summary>
		/// Resume execution of this task on the given lua state.
		/// Is this is not the same state previously passed to Exec, behaviour is undefined.
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Resume(lua_State* pL);*/

		/// <summary>
		/// Get execution status of this Task
		/// </summary>
		/// <returns>Current status</returns>
		TaskStatus GetStatus()
		{
			if (mTask == nullptr) return TaskStatus::Error;
			return mTask->GetStatus();
		}


		/// <summary>
		/// Permanently cancel execution and any pending tasks
		/// </summary>
		void Cancel()
		{
			if (mTask == nullptr) return;
			mTask->Cancel();
		}

	};
}
#endif