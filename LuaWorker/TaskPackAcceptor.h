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

#ifndef _TASK_PACK_ACCEPTOR_H_
#define _TASK_PACK_ACCEPTOR_H_
#pragma once

#include <memory> 

namespace LuaWorker
{
	class OneShotTaskExecPack;
	class CoTaskExecPack;

	/// <summary>
	/// Intereface for executing type of task exec pack 
	/// </summary>
	class TaskPackAcceptor
	{

	public:
		/// <summary>
		/// Exec task in lua state. 
		/// Call in worker thread only.
		/// </summary>
		/// <param name="task">Task to execute</param>
		/// <param name="resumeToken">Resume token output</param>
		virtual void ExecTask(std::unique_ptr <OneShotTaskExecPack>&& task) = 0;

		/// <summary>
		/// Exec task in lua state. 
		/// Call in worker thread only.
		/// </summary>
		/// <param name="task">Task to execute</param>
		/// <param name="resumeToken">Resume token output</param>
		virtual void ExecTask(std::unique_ptr <CoTaskExecPack>&& task) = 0;
	};
}

#endif