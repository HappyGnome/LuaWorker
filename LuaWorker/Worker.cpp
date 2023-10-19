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

#include "Worker.h"
#include "TaskExecPack.h"

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

std::optional<TaskExecPack> Worker::RunCurrentTasks(InnerLuaState& lua)
{
	while (!mCancel)
	{
		{
			std::unique_lock<std::mutex> lock(mTasksMtx);

			if (!mTaskQueue.empty())
			{
				std::optional<TaskExecPack> newTaskOut = std::make_optional(std::move(mTaskQueue.front()));
				mTaskQueue.pop_front();
				return newTaskOut;
			}
		}

		if (!lua.IsOpen())
		{
			mLog.Push(LogLevel::Error, "Lua not initialized.");
			break;
		}

		lua.ResumeTask();

		std::optional<std::chrono::system_clock::time_point> nextResume = lua.GetNextResume();

		if (nextResume.has_value() && nextResume.value() > std::chrono::system_clock::now())
		{
			std::unique_lock<std::mutex> lock(mTasksMtx);
			mTaskCancelCv.wait_until(lock, nextResume.value());
		}
	}

	return std::optional<TaskExecPack>();
}

//------
void Worker::ThreadMainLoop(InnerLuaState& lua)
{

	try
	{

		while (!mCancel)
		{
			std::optional<TaskExecPack> currentTask (RunCurrentTasks(lua));
			
			if (!currentTask.has_value()) break;

			if (mCancel) break;
			else if (currentTask.value().GetStatus() != TaskStatus::NotStarted) continue;

			if (!lua.IsOpen())
			{
				mLog.Push(LogLevel::Error, "Lua not initialized.");
				break;
			}

			lua.ExecTask(std::move(currentTask.value()));
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
	TaskExecPack pack(task, LogSection(mLog));

	{
		std::unique_lock<std::mutex> lock(mTasksMtx);

		if (mCancel)
		{
			task->Cancel();
			return mCurrentStatus;
		}

		mTaskQueue.push_back(std::move(pack));
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