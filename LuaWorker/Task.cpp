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

#include "Task.h"

#include <chrono>

using namespace std::chrono_literals;
using std::chrono::system_clock;

using namespace LuaWorker;

//-------------------------------
// Private methods
//-------------------------------

void Task::SetResult(const std::string& newResult)
{

	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		mResult = newResult;
	}
}

//-------------------------------
// Protected methods
//-------------------------------

void Task::SleepFor(unsigned int waitForMillis)
{
	system_clock::time_point sleepTill = system_clock::now() + (waitForMillis * 1ms);

	while (system_clock::now() < sleepTill)
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus == TaskStatus::Cancelled
			|| mStatus == TaskStatus::Error) return;

		mResultStatusCv.wait_for(lock, sleepTill - system_clock::now());

	}

}

//-------------------------------
// Public static methods
//-------------------------------


bool Task::IsFinal(TaskStatus status)
{
	return (status == TaskStatus::Cancelled
		|| status == TaskStatus::Complete
		|| status == TaskStatus::Error);
}

//-------------------------------
// Public methods
//-------------------------------

std::string Task::GetResult()
{

	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		return mResult;
	}
}

//------
void Task::WaitForResult(unsigned int waitForMillis)
{
	if (waitForMillis <= 0) waitForMillis = 1;

	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (IsFinal(mStatus)) return;

		mResultStatusCv.wait_for(lock, waitForMillis * 1ms);

	}
}

//------
TaskStatus Task::GetStatus()
{
	std::unique_lock<std::mutex> lock(mResultStatusMtx);

	return mStatus;
}


//------
Task::Task() : mStatus(TaskStatus::NotStarted) {}

//------
void Task::Exec(lua_State* pL)
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus != TaskStatus::NotStarted) return;

		mStatus = TaskStatus::Running;
	}

	mResultStatusCv.notify_all();
	SetResult(this -> DoExec(pL));

	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus != TaskStatus::Error) mStatus = TaskStatus::Complete;
	}

	mResultStatusCv.notify_all();
}

//------
void Task::SetError(const std::string& errMsg)
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if(mStatus != TaskStatus::Error) mError = errMsg; // Keep first error
		mStatus = TaskStatus::Error;
	}
	mResultStatusCv.notify_all();
}

//------
std::string Task::GetError()
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		return mError;
	}
}

//------
void Task::Cancel()
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		mStatus = TaskStatus::Cancelled;
	}
	mResultStatusCv.notify_all();
}
