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

#ifndef _INNER_LUA_STATE_H_
#define _INNER_LUA_STATE_H_
#pragma once

#include <mutex> 
#include <chrono> 

#include "Cancelable.h"
#include "AutoKeyLoanDeck.h"

#include "LogSection.h"
//#include "OneShotTaskExecPack.h"
//#include "CoTaskExecPack.h"
#include "TaskPackAcceptor.h"

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
	class InnerLuaState : public Cancelable, public TaskPackAcceptor
	{
	private:

		typedef AutoKeyDeck::AutoKeyLoanDeck<std::unique_ptr<CoTaskExecPack>,std::chrono::system_clock::time_point,int> T_SuspendedTaskDeck;
		typedef T_SuspendedTaskDeck::CardType T_SuspendedTaskCard;

		/// <summary>
		/// Lua registry key to InnerLuaState instance pointer
		/// Use address of this as the actual key to instance data table in the lua registry 
		/// (as there's a different lua state per instance, there won't be clashes between instances)
		/// </summary>
		const static char* cLuaRegistryThisKey;

		/// <summary>
		/// Lua registry key to table of instance threads
		/// Use address of this as the actual key to instance data table in the lua registry 
		/// (as there's a different lua state per instance, there won't be clashes between instances)
		/// </summary>
		const static char* cLuaRegistryThreadTableKey;
		const static char* cInLuaWorkerTableName;


		std::atomic<bool> mCancel;
		std::atomic<bool> mOpen;

		std::mutex mCancelMtx;
		std::condition_variable mCancelCv;

		LogSection mLog;

		//Access in worker thread only
		lua_State* mLua;

		//Access in worker thread only
		T_SuspendedTaskDeck mResumableTasks;

		std::chrono::system_clock::time_point mResumeCurrentTaskAt;
		bool mCurrentTaskYielded;
		bool mCurrentTaskCanYield;

		//---------------------
		// Private methods
		//---------------------

		/// <summary>
		/// Call from worker thread only
		/// The thread on which the task runs must be at the top of the stack for the internal state (mLua)
		/// If there is a thread at index -1, it is popped
		/// <param name="task">Task to push</param>
		/// <param name="resumeAt">Target resume time for this task</param>
		/// </summary>
		bool HandleSuspendedTask(std::unique_ptr<CoTaskExecPack>&& task, std::chrono::system_clock::time_point resumeAt);

		lua_State* GetTaskThread(int taskHandle);

		void RemoveTaskThread(int taskHandle);

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
		///		InLuaWorker.YieldFor( millis, resultString )
		/// </summary>
		/// <param name="pL"></param>
		/// <returns></returns>
		static int l_YieldFor(lua_State* pL);

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
		/// Call in worker thread only.
		/// </summary>
		~InnerLuaState();

		/// <summary>
		/// Open lua state
		/// Call in worker thread only.
		/// </summary>
		void Open();

		/// <summary>
		/// Close lua state
		/// Call in worker thread only.
		/// </summary>
		void Close();

		/// <summary>
		/// Exec task in lua state. 
		/// Call in worker thread only.
		/// </summary>
		/// <param name="task">Task to execute</param>
		/// <param name="resumeToken">Resume token output</param>
		void ExecTask(std::unique_ptr <OneShotTaskExecPack>&& task);

		/// <summary>
		/// Exec task in lua state. 
		/// Call in worker thread only.
		/// </summary>
		/// <param name="task">Task to execute</param>
		/// <param name="resumeToken">Resume token output</param>
		void ExecTask(std::unique_ptr <CoTaskExecPack>&& task);

		/// <summary>
		/// Resume task from a token
		/// </summary>
		/// <param name="resumeToken">Handle to task to return</param>
		void ResumeTask();

		/// <summary>
		/// Get time of next resumable task in queue
		/// Call in worker thread only.
		/// </summary>
		std::optional<std::chrono::system_clock::time_point> GetNextResume();

		/// <summary>
		/// Raise cancel flag (ExecTask will throw a LuaCancellationException at next hook event)
		/// Can be called from any thread
		/// </summary>
		void Cancel();

		/// <summary>
		/// Get whether lua state is open
		/// Can be called from any thread
		/// </summary>
		/// <returns></returns>
		bool IsOpen();
	};
}

#endif