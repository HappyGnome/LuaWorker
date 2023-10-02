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

#include "Worker.h"

using namespace LuaWorker;

//-------------------------------
// Private methods
//-------------------------------

void Worker::ThreadMain() 
{
	mLog.Push(LogLevel::Info, "Thread starting.");

	lua_State* pL = ThreadMainInitLua();

	mLog.Push(LogLevel::Info, "Lua opened on worker.");

	ThreadMainLoop(pL);

	mLog.Push(LogLevel::Info, "Thread stopping.");

	Cancel(); // Ensure tasks cancelled and status updated

	if (pL != nullptr) lua_close(pL);
}

//------
lua_State* Worker::ThreadMainInitLua()
{
	try
	{
		mCurrentStatus = WorkerStatus::Processing;
		lua_State* pL = lua_open();
		luaL_openlibs(pL);

			lua_createtable(pL, 0, 2);
				lua_pushlightuserdata(pL, this);
				lua_pushcclosure(pL, Worker::l_LogError, 1);
			lua_setfield(pL, -2, "LogError");
				lua_pushlightuserdata(pL, this);
				lua_pushcclosure(pL, Worker::l_LogInfo, 1);
			lua_setfield(pL, -2, "LogInfo");

		lua_setglobal(pL, "LuaWorker");

		return pL;
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mCurrentStatus = WorkerStatus::Error;
	}

	return nullptr;
}

//------
void Worker::ThreadMainLoop(lua_State* pL)
{
	if (pL == nullptr)
	{
		mLog.Push(LogLevel::Error, "Lua not initialized.");
		return;
	}

	try
	{
		std::shared_ptr<Task> currentTask;

		while (!mCancel)
		{
			while (!mCancel)
			{
				std::unique_lock<std::mutex> lock(mTasksMtx);

				if (!mTaskQueue.empty())
				{
					currentTask = mTaskQueue.front();
					mTaskQueue.pop_front();
					break;
				}

				mTaskCancelCv.wait(lock);
			}

			if (mCancel) break;
			else if (currentTask == nullptr || currentTask->GetStatus() != TaskStatus::NotStarted) continue;

			try
			{
				currentTask->Exec(pL);

				if (currentTask->GetStatus() == TaskStatus::Error) mLog.Push(LogLevel::Error, currentTask->GetError());

				currentTask = nullptr;
			}
			catch (const std::exception& ex)
			{
				mLog.Push(ex);
				currentTask->SetError("Exeception occurred during execution");
			}
		}
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mCurrentStatus = WorkerStatus::Error;
	}
}

//------
void Worker::Cancel()
{
	mCancel = true;
	if (mCurrentStatus != WorkerStatus::Error) mCurrentStatus = WorkerStatus::Cancelled;

	CancelAllTasks();

	mTaskCancelCv.notify_all();
}

//------
void Worker::CancelAllTasks()
{
	std::unique_lock<std::mutex> lock(mTasksMtx);

	for (auto it = mTaskQueue.begin(); it != mTaskQueue.end(); ++it)
	{
		(*it)->Cancel();
	}

	mTaskQueue.clear();
}

//---------------------
// Worker Lua 
// C methods
//---------------------

Worker* Worker::l_PopThis(lua_State* pL)
{
	if (!lua_islightuserdata(pL, lua_upvalueindex(1))) return nullptr;
	return (Worker*)lua_topointer(pL, lua_upvalueindex(1));
}

int Worker::l_Log(lua_State* pL, LogLevel level)
{
	Worker* pWorker = l_PopThis(pL);

	if (pWorker != nullptr)
	{
		if (!lua_isstring(pL, -1)) return 0;
		pWorker->mLog.Push(level, lua_tostring(pL, -1));
	}
	return 0;
}

int Worker::l_LogError(lua_State* pL)
{
	return l_Log(pL, LogLevel::Error);
}

//------
int Worker::l_LogInfo(lua_State* pL)
{
	return l_Log(pL, LogLevel::Info);
}


//-------------------------------
// Public methods
//-------------------------------

Worker::Worker(LogSection && log) : mCancel(false), mCurrentStatus(WorkerStatus::NotStarted), mLog(log){}

//------
Worker::~Worker()
{
	try
	{
		if (mThread.joinable())
		{
			mLog.Push(LogLevel::Warn, "Stop not called on thread. Attempting shutdown...");
			Stop();
			//Perhaps add a timeout here, or where the thread calls Exec.
		}
	}
	catch (const std::exception& ex)
	{
		//Suppress exceptions from destructor
	}
}

//------
WorkerStatus Worker::Start()
{
	if (!mThread.joinable() && !mCancel) 
	{
		mLog.Push(LogLevel::Info, "Thread start requested.");

		std::unique_lock<std::mutex> lock(mTasksMtx);

		if(!mCancel) mThread = std::thread( &Worker::ThreadMain, this);

		mCurrentStatus = WorkerStatus::Starting;
	}
	return mCurrentStatus;
}

//------
WorkerStatus Worker::Stop()
{
	mLog.Push(LogLevel::Info, "Thread stop requested.");

	Cancel();
	if (mThread.joinable())
	{
		mThread.join();
	}
	return mCurrentStatus;
}

//------
WorkerStatus Worker::AddTask(std::shared_ptr<Task> task)
{	
	{
		std::unique_lock<std::mutex> lock(mTasksMtx);

		if (mCancel)
		{
			task->Cancel();
			return mCurrentStatus;
		}

		mTaskQueue.push_back(task);
	}
	mTaskCancelCv.notify_all();

	return mCurrentStatus;
}

//------
WorkerStatus Worker::GetStatus()
{
	return mCurrentStatus;
}
