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

#ifndef _INNER_LUA_STATE_H_
#define _INNER_LUA_STATE_H_

#include <mutex> 
//#include <map> 

#include "LogSection.h"
#include "Task.h"
#include "Cancelable.h"

extern "C" {
#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
}


namespace LuaWorker
{

	/// <summary>
	/// Manages lua state in worker thread
	/// </summary>
	class InnerLuaState : public Cancelable
	{
	private:

		const static char* cLuaStateHandleKey;

		std::atomic<bool> mCancel;
		std::atomic<bool> mOpen;

		LogSection mLog;

		lua_State* mLua;
		std::mutex mLuaMtx;

		//---------------------
		// InnerLuaState
		// Lua C methods
		//---------------------
		static InnerLuaState* l_PopThis(lua_State* pL);
		static int l_Log(lua_State* pL, LogLevel level);
		static int l_LogError(lua_State* pL);
		static int l_LogInfo(lua_State* pL);
		static void l_Hook(lua_State* pL, lua_Debug *pDebug);

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="log">Logger to use</param>
		InnerLuaState(LogSection&& log);

		/// <summary>
		/// Destructor
		/// </summary>
		~InnerLuaState();

		/// <summary>
		/// Open lua state
		/// </summary>
		void Open();

		/// <summary>
		/// Close lua state
		/// </summary>
		void Close();

		/// <summary>
		/// Exec task in lua state
		/// </summary>
		/// <param name="task"></param>
		void ExecTask(std::shared_ptr<Task> task);

		/// <summary>
		/// Raise cancel flag (ExecTask will throw a LuaCancellationException at next hook event)
		/// </summary>
		void Cancel();

		/// <summary>
		/// Get whether lua state is open
		/// </summary>
		/// <returns></returns>
		bool IsOpen();
	};
}
#endif