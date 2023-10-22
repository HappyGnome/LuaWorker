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

#include "CoTask.h"
#include "CoTaskExecPack.h"

using namespace LuaWorker;

void CoTaskExecPack::Exec(lua_State* pL)
{
	if (mTask == nullptr) return;

	try
	{
		mTask->Exec(pL);
		if (mTask->GetStatus() == TaskStatus::Error)
			mLog.Push(LogLevel::Error, mTask->GetError());
	}
	catch (const std::exception& ex)
	{
		mLog.Push(ex);
		mTask->SetError("Exeception occurred during execution");
	}
}

void CoTaskExecPack::Resume(lua_State* pL)
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
