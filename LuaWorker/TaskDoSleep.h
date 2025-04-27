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

#ifndef _TASK_SLEEP_H_
#define _TASK_SLEEP_H_
#pragma once

#include<chrono>

#include "OneShotTask.h"

extern "C" {
#include "lua.h"
	//#include "lauxlib.h"
	//#include "lualib.h"
}

namespace LuaWorker
{
	/// <summary>
	/// Implementation of Task that sleeps for a minimum amount of time
	/// </summary>
	class TaskDoSleep : public OneShotTask
	{
	private:

		unsigned int mSleepForMs;

	protected:

		/// <summary>
		/// Do the lua work for this task
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Result of the task</returns>
		LuaArgBundle DoExec(lua_State* pL) override;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="sleepForMs">Time to sleep for</param>
		explicit TaskDoSleep(unsigned int sleepForMs, TaskConfig&& config);
	};
};
#endif