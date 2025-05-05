/*****************************************************************************\
*
*  Copyright 2025 HappyGnome
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
#include <assert.h>

using namespace std::chrono_literals;
using std::chrono::system_clock;

using namespace LuaWorker;

//-------------------------------
// Private methods
//-------------------------------
Task::Task(const std::string& data, const TaskConfig& config) : mConfig(config), mExecData(data){}

Task::Task(unsigned int data, const TaskConfig& config): mConfig(config), mExecData(data) {}

Task::Task(const std::string& funcString, const LuaArgBundle& args, const TaskConfig& config) : mConfig(config), mExecData(CoroutineData{funcString, args}) {}


LuaArgBundle Task::DoExecFile(lua_State* pL) 
{
	assert(mType == TaskType::DoFile);

	int prevTop = lua_gettop(pL);
	int execResult = luaL_dofile(pL, std::get<std::string>(mExecData).c_str());

	int resC = 0;

	if (execResult != 0)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING) 
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error in file " + std::get<std::string>(mExecData) + ": " + luaError);
	}
	else
	{
		resC = lua_gettop(pL) - prevTop;
	}
	
	LuaArgBundle results(pL, resC, GetMaxTableDepth());

	lua_settop(pL, 0);

	return results;
}

LuaArgBundle Task::DoExecSleep(lua_State* pL)
{
	assert(mType == TaskType::DoSleep);
	
	SleepFor(std::get<unsigned int> (mExecData));

	return LuaArgBundle();
}

LuaArgBundle Task::DoExecString(lua_State* pL)
{
	assert(mType == TaskType::DoString);

	int resC = 0;
	int prevTop = lua_gettop(pL);
	int execResult = luaL_dostring(pL, std::get<std::string>(mExecData).c_str());

	if (execResult != 0)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error in lua string: " + luaError);
	}
	else
	{
		resC = lua_gettop(pL) - prevTop;
	}

	LuaArgBundle result(pL, resC, GetMaxTableDepth());

	lua_settop(pL, 0);

	return result;
}

LuaArgBundle Task::DoExecCoroutine(lua_State* pL)
{
	CoroutineData data = std::get<CoroutineData>(mExecData);

	int execResult = luaL_dostring(pL,data.FuncString.c_str());

	if (execResult != 0)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error in lua string: " + luaError);
	}

	int argC = 0;

	argC = data.Args.Unpack(pL);

	LuaArgBundle result;

	if (!lua_isfunction(pL, -argC - 1))
	{
		SetError("Invalid coroutine construction.");
		lua_settop(pL, 0);

	}
	else result = DoResumeCoroutine(pL, argC, 1);

	return  result;
}

LuaArgBundle Task::DoResumeCoroutine(lua_State* pL, int argC, int funcCall)
{

	int prevTop = lua_gettop(pL) - argC - funcCall;

	int execResult = lua_resume(pL, argC);	
	
	LuaArgBundle results;

	if (execResult != 0 && execResult != LUA_YIELD)
	{
		std::string luaError = "No Error Message!";
		if (lua_type(pL, -1) == LUA_TSTRING)
		{
			luaError = lua_tostring(pL, -1);
		}

		SetError("Error resuming task: " + luaError);

	}
	else
	{
		int resC = lua_gettop(pL);// -prevTop;
		results = LuaArgBundle(pL, resC, GetMaxTableDepth());
	}
		
	lua_settop(pL, 0);

	return results;
}


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


std::shared_ptr<Task> Task::NewDoFile(const std::string& filePath, const TaskConfig& config)
{
	std::shared_ptr<Task> result(new Task(filePath, config));

	result->mType = TaskType::DoFile;

	return result;
}


std::shared_ptr<Task> Task::NewDoSleep(unsigned int sleepForMs, const TaskConfig& config)
{
	std::shared_ptr<Task> result(new Task(sleepForMs, config));

	result->mType = TaskType::DoSleep;

	return result;
}

std::shared_ptr<Task> Task::NewDoString(const std::string& luaString, const TaskConfig& config)
{
	std::shared_ptr<Task> result(new Task(luaString, config));
	result->mType = TaskType::DoString;

	return result;
}

std::shared_ptr<Task> Task::NewDoCoroutine(const std::string& funcString,  const LuaArgBundle& args, const TaskConfig& config)
{
	std::shared_ptr<Task> result(new Task("return " + funcString, args, config));
	result->mType = TaskType::Coroutine;

	return result;
}

//-------------------------------
// Public methods
//-------------------------------

//Task::Task(std::shared_ptr<Task> other) noexcept
//{
//	*this = std::move(other);
//}
//
//Task& Task::operator= (std::shared_ptr<Task> other) {
//	mStatus = other.mStatus;
//
//	mResult = std::move(other.mResult);
//	mError = std::move(other.mError);
//	mExecData = std::move(other.mExecData);
//	mResultStatusCv = other.mResultStatusCv;
//
//		std::mutex mResultStatusMtx;
//		std::condition_variable mResultStatusCv;
//
//		bool mUnreadResult;
//
//		TaskConfig mConfig;
//
//		TaskType mType;
//
//		std::variant<std::string, unsigned int, CoroutineData> mExecData;
//
//}

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
//Task::Task( const TaskConfig& config) : mStatus(TaskStatus::NotStarted), mUnreadResult(false), mConfig(config) 
//{
//#ifdef _BENCHMARK_OBJ_COUNTERS_
//	CountPushed++;
//	if (CountPushed - CountDeleted > PeakTaskCount)
//	{
//		PeakTaskCount = CountPushed - CountDeleted;
//	}
//#endif
//}

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

void Task::Exec(lua_State* pL)
{
	if (pL == nullptr || !TrySetRunning()) return;


	LuaArgBundle result;
	bool yielded = false;

	switch (mType)
	{
	case TaskType::DoFile: 
		result = DoExecFile(pL);
		break;
	case TaskType::DoSleep:
		result = DoExecSleep(pL);
		break;
	case TaskType::DoString:
		result = DoExecString(pL);
		break;
	case TaskType::Coroutine:
		result = DoExecCoroutine(pL);
		break;
	default:
		break;
	}

	SetResult(std::move(result), lua_status(pL) == LUA_YIELD);
}

void Task::Resume(lua_State* pL)
{
	assert(mType == Coroutine);

	if (pL == nullptr || lua_status(pL) != LUA_YIELD || !TrySetRunning(TaskStatus::Suspended)) return;

	LuaArgBundle results = DoResumeCoroutine(pL, 0, 0);

	SetResult(std::move(results), lua_status(pL) == LUA_YIELD);
}

bool Task::RunOnOwnLuaThread() const
{
	return mType == TaskType::Coroutine;
}

//------------------------------
// Diagnostic statics
//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		std::atomic<int> Task::CountPushed = 0;
		std::atomic<int> Task::CountDeleted = 0;
		std::atomic<int> Task::PeakTaskCount = 0;
#endif


