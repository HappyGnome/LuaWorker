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
// Protected methods
//-------------------------------

void Task::SetResult(LuaArgBundle&& results, bool yielded)
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		mUnreadResult = true;
		mResult = std::move(results);

		if (mStatus != TaskStatus::Error)
		{
			if (yielded)
			{
				mStatus = TaskStatus::Suspended;
			}
			else mStatus = TaskStatus::Complete;
		}
	}

	mResultStatusCv.notify_all();
}

bool Task::TrySetRunning(TaskStatus expected)
{
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus != expected) return false;

		mStatus = TaskStatus::Running;
	}
	mResultStatusCv.notify_all();
	return true;
}


void Task::SleepFor(unsigned int waitForMillis)
{
	system_clock::time_point sleepTill = system_clock::now() + (waitForMillis * 1ms);

	while (system_clock::now() < sleepTill)
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus == TaskStatus::Cancelled
			|| mStatus == TaskStatus::Error) return;

		mResultStatusCv.wait_until(lock, sleepTill);

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

int Task::GetResult(lua_State *pL)
{

	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);
		mUnreadResult = false;
		return mResult.Unpack(pL);
	}
}

//------
bool Task::WaitForResult(long waitForMillis)
{
	if (waitForMillis <= 0)
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		return (mStatus == TaskStatus::Complete || mUnreadResult);

	}

	system_clock::time_point sleepTill = system_clock::now() + (waitForMillis * 1ms);

	while (system_clock::now() < sleepTill)
	{
		std::unique_lock<std::mutex> lock(mResultStatusMtx);

		if (mStatus == TaskStatus::Complete || mUnreadResult)
		{
			return true;
		}
		else if (IsFinal(mStatus))
		{
			return false;
		}

		mResultStatusCv.wait_until(lock, sleepTill);

	}
	return false;
}

//------
TaskStatus Task::GetStatus()
{
	std::unique_lock<std::mutex> lock(mResultStatusMtx);

	return mStatus;
}


//------
Task::Task( TaskConfig&& config) : mStatus(TaskStatus::NotStarted), mUnreadResult(false), mConfig(config) 
{
#ifdef _BENCHMARK_OBJ_COUNTERS_
	CountPushed++;
	if (CountPushed - CountDeleted > PeakTaskCount)
	{
		PeakTaskCount = CountPushed - CountDeleted;
	}
#endif
}

//------
#ifdef _BENCHMARK_OBJ_COUNTERS_
Task::~Task()
{
	CountDeleted++;
}
#endif


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

		if (!IsFinal(mStatus)) mStatus = TaskStatus::Cancelled;
	}
	mResultStatusCv.notify_all();
}

//------
int Task::GetMaxTableDepth() 
{
	return mConfig.MaxTableDepth;
}

//------------------------------
// Diagnostic statics
//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		std::atomic<int> Task::CountPushed = 0;
		std::atomic<int> Task::CountDeleted = 0;
		std::atomic<int> Task::PeakTaskCount = 0;
#endif


