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

#include<functional>

#include "TaskExecPack.h"
#include "LogSection.h"
#include "InnerLuaState.h"
#include "Worker.h"
#include "Task.h"
//#include "OneShotTaskExecPack.h"
//#include "CoTaskExecPack.h"
//#include "OneShotTask.h"
//#include "CoTask.h"

using namespace LuaWorker;

//-------------------------------
// Private methods
//-------------------------------

void Worker::ThreadMain()
{
	mLog.Push(LogLevel::Info, "Thread starting.");

	InnerLuaState lua(mLog);

	if (ThreadMainInitLua(lua)) mLog.Push(LogLevel::Info, "Lua opened on worker.");

	ThreadMainLoop(lua);

	mLog.Push(LogLevel::Info, "Thread stopping.");

	Cancel(); // Ensure tasks cancelled and status updated

	ThreadMainCloseLua(lua);
}

//------
bool Worker::ThreadMainInitLua(InnerLuaState& lua)
{
	try
	{
		mCurrentStatus = WorkerStatus::Processing;

		{
			std::unique_lock<std::mutex> lock(mLuaCancelMtx);
			mLuaCancel = &lua;
		}
		
		lua.Open();

		return true;
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mCurrentStatus = WorkerStatus::Error;
	}
	return false;
}

//------
bool Worker::ThreadMainCloseLua(InnerLuaState& lua)
{
	try
	{
		lua.Close();

		{
			std::unique_lock<std::mutex> lock(mLuaCancelMtx);
			mLuaCancel = nullptr;
		}

		return true;
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mCurrentStatus = WorkerStatus::Error;
	}
	return false;
}

std::shared_ptr<Task> Worker::RunCurrentTasks(InnerLuaState& lua)
{
	while (!mCancel)
	{
		std::optional<std::chrono::system_clock::time_point> nextResume = lua.GetNextResume();

		{
			std::unique_lock<std::mutex> lock(mTasksMtx);
			while (!mCancel)
			{
				if (!mTaskQueue.empty())
				{
					std::shared_ptr<Task> newTaskOut(mTaskQueue.front());
					mTaskQueue.pop_front();
					return newTaskOut;
				}

				if (!nextResume.has_value())
				{
					mTaskCancelCv.wait(lock);
				}
				else if (nextResume.value() > std::chrono::system_clock::now())
				{
					mTaskCancelCv.wait_until(lock, nextResume.value());
				}
				else break; // Tasks to resume
			}
		}

		if (mCancel) break;

		if (!lua.IsOpen())
		{
			mLog.Push(LogLevel::Error, "Lua not initialized.");
			break;
		}

		lua.ResumeTask();
	}

	return nullptr;
}

//------
void Worker::ThreadMainLoop(InnerLuaState& lua)
{
	try
	{
		while (!mCancel)
		{
			std::shared_ptr<Task> currentTask = RunCurrentTasks(lua);
			
			if (currentTask == nullptr) break;

			if (mCancel) break;
			else if (currentTask->GetStatus() != TaskStatus::NotStarted) continue;

			if (!lua.IsOpen())
			{
				mLog.Push(LogLevel::Error, "Lua not initialized.");
				break;
			}

			lua.ExecTask(currentTask);
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

	//CancelAllTasks();

	mTaskCancelCv.notify_all();

	{
		std::unique_lock<std::mutex> lock(mLuaCancelMtx);
		if (mLuaCancel != nullptr) mLuaCancel->Cancel();
	}
}

//------
//void Worker::CancelAllTasks()
//{
//	std::unique_lock<std::mutex> lock(mTasksMtx);
//
//	for (auto it = mTaskQueue.begin(); it != mTaskQueue.end(); ++it)
//	{
//		(*it).Cancel();
//	}
//
//	mTaskQueue.clear();
//}


//-------------------------------
// Public methods
//-------------------------------

Worker::Worker(LogSection && log) : mCancel(false), 
									mCurrentStatus(WorkerStatus::NotStarted), 
									mLog(log), 
									mLuaCancel(nullptr){}

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
	catch (const std::exception&)
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

	Worker::Cancel();
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

//------
std::shared_ptr<LogOutput> Worker::GetLogOutput()
{
	return mLog.GetLogOutput();
}