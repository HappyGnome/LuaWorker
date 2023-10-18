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

#include <memory>

#include "TaskExecPack.h"

using namespace LuaWorker;


TaskExecPack::TaskExecPack(const std::shared_ptr<Task>& task, LogSection&& log) :
	mTask(task), mLog(log) {}

TaskExecPack::~TaskExecPack() 
{
	if (mTask == nullptr) return;

	try
	{
		mTask->Cancel();
	}
	catch (const std::exception& ex)
	{
		// Suppress exception in destructor
	}
}

void TaskExecPack::Exec(lua_State* pL)
{
	if (mTask == nullptr) return;

	try
	{
		mTask->Exec(pL);
		if(mTask->GetStatus() == TaskStatus::Error) 
			mLog.Push(LogLevel::Error, mTask->GetError());
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mTask->SetError("Exeception occurred during execution");
	}
}

void TaskExecPack::Resume(lua_State* pL)
{
	if (mTask == nullptr) return;

	try
	{
		mTask->Resume(pL);
		if (mTask->GetStatus() == TaskStatus::Error)
			mLog.Push(LogLevel::Error, mTask->GetError());
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mTask->SetError("Exeception occurred during execution");
	}
}


TaskStatus TaskExecPack::GetStatus()
{
	if (mTask == nullptr) return TaskStatus::Error;
	return mTask->GetStatus();
}


void TaskExecPack::Cancel()
{
	if (mTask == nullptr) return;
	mTask->Cancel();
}

