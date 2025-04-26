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

#ifndef _TASKDOSTRING_H_
#define _TASKDOSTRING_H_
#pragma once

#include<string>

#include "OneShotTask.h"

extern "C" {
#include "lua.h"
	//#include "lauxlib.h"
	//#include "lualib.h"
}

namespace LuaWorker
{
	/// <summary>
	/// Implementation of Task that executes a lua string
	/// </summary>
	class TaskDoString : public OneShotTask
	{
	private:

		std::string mExecString;

	protected:

		/// <summary>
		/// Do the lua work for this task
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Args returned</returns>
		LuaArgBundle DoExec(lua_State* pL) override;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="execString">Lua string to run when this task executes</param>
		explicit TaskDoString(std::string execString);
	};
};
#endif
