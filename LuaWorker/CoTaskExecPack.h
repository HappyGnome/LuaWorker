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

#ifndef _CO_TASK_EXEC_PACK_H_
#define _CO_TASK_EXEC_PACK_H_
#pragma once

#include <memory>

#include "LogSection.h"
#include "TypedTaskExecPack.h"
#include "CoTask.h"

//#include "TaskPackAcceptor.h"

namespace LuaWorker
{
	/// <summary>
	/// Exec pack for a coroutine class
	/// </summary>
	class CoTaskExecPack : public TypedTaskExecPack<CoTask, CoTaskExecPack> {

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		using TypedTaskExecPack::TypedTaskExecPack;


		/// <summary>
		/// Execute this task on a given lua state
		/// </summary>
		/// <param name="pL">Lua state</param>
		void Exec(lua_State* pL);

		/// <summary>
		/// Resume yielded task 
		/// </summary>
		/// <param name="pL">The thread on which Exec was previously called</param>
		void Resume(lua_State* pL);

	};
}

#endif