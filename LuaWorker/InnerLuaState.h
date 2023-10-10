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
#include <chrono> 

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
		const static char* cInLuaWorkerTableName;
		const static char* cInLuaWorkerThreadsTableName;

		std::atomic<bool> mCancel;
		std::atomic<bool> mOpen;

		std::mutex mCancelMtx;
		std::condition_variable mCancelCv;

		LogSection mLog;

		//Access in worker thread only
		lua_State* mLua;

		//Access in worker thread only
		std::chrono::duration<float> mSuspendCurrentTaskFor;

		//---------------------
		// Private
		//---------------------

		/// <summary>
		/// Call from worker thread only
		/// <param name="pThread">Coroutine in which the task was running</param>
		/// <param name="resumeAfter">If this is set to a positive value the task should be sheduled to resume after this delay</param>
		/// </summary>
		bool HandleSuspendedTask(lua_State* pThread, std::chrono::duration<float>& resumeAfter);

		//---------------------
		// InnerLuaState
		// Lua C methods
		//---------------------
		static InnerLuaState* l_PopThis(lua_State* pL);
		static int l_Log(lua_State* pL, LogLevel level);
		static int l_LogError(lua_State* pL);
		static int l_LogInfo(lua_State* pL);

		static int l_Sleep(lua_State* pL);

		static void l_Hook(lua_State* pL, lua_Debug *pDebug);

		/// <summary>
		/// 
		/// Lua syntax:
		///		InLuaWorker.ResumeIn( millis )
		/// </summary>
		/// <param name="pL"></param>
		/// <returns></returns>
		static int l_ResumeIn(lua_State* pL);

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="log">Logger to use</param>
		explicit InnerLuaState(LogSection&& log);

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="log">Logger to copy</param>
		explicit InnerLuaState(const LogSection& log);

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
		/// Exec task in lua state. Call in worker thread only.
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