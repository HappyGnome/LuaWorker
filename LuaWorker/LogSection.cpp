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

#include "LogSection.h"

using namespace LuaWorker;

LogSection::LogSection(std::shared_ptr<LogStack> stack, std::string section) : mLog(stack), mSection(section) {}

LogSection::LogSection(const LogSection& other) : mLog(other.mLog), mSection (other.mSection) {}

LogSection& LogSection::operator= (const LogSection& other)
{
	mLog = other.mLog;
	mSection = other.mSection;

	return *this;
}

void LogSection::Push(const LogLevel& level, const std::string& message)
{
	if (mLog != nullptr)
	{
		mLog->Push(LogItem(level,message, mSection));
	}
}

void LogSection::Push(const std::exception& e)
{
	Push ( LogLevel::Error, std::string(e.what()));
}