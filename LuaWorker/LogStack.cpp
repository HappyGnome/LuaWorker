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

#include "LogStack.h"

using namespace LuaWorker;

LogStack::LogStack(std::size_t capacity) : mCapacity(capacity) {}

bool LogStack::PopLine(std::string& message) 
{
	std::lock_guard<std::mutex> guard(mMessagesMutex);

	if (mMessages.empty()) return false;

	message = mMessages.back().ToString();
	mMessages.pop_back();

	return true;
}

void LogStack::Push(const LogItem& message)
{
	std::lock_guard<std::mutex> guard(mMessagesMutex);
	if (mMessages.size() >= mCapacity) mMessages.pop_back();
	mMessages.push_front(message);
}