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

#ifndef _TASKDOFILE_H_
#define _TASKDOFILE_H_

#include<string>

#include "Task.h"

extern "C" {
#include "lua.h"
	//#include "lauxlib.h"
	//#include "lualib.h"
}

namespace LuaWorker
{
	/// <summary>
	/// Implementation of Task that executes a lua file
	/// </summary>
	class TaskDoFile : public Task
	{
	private:

		std::string mFilePath;

	protected:

		/// <summary>
		/// Do the lua work for this task
		/// </summary>
		/// <param name="pL">Lua state</param>
		/// <returns> Result of the task</returns>
		std::string DoExec(lua_State* pL) override;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="filePath">Filepath to run when this task executes</param>
		explicit TaskDoFile(std::string filePath);
	};
};
#endif
