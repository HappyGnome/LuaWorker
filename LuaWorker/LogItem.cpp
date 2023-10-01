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

#include <string>
#include <ctime>

#include "LogItem.h"

using namespace LuaWorker;

//---------------------
// Public methods
//---------------------


LogItem::LogItem(LogLevel level, std::string message, std::string section) : mLevel(level), mMessage(message), mSection(section) 
{
	mTime = std::time(nullptr);
}

std::string LogItem::ToString()
{
	std::string sep = " | ";

	char timestampStr[std::size("yyyy-MM-dd hh:mm:ssZ")];
	tm tmBuf;	
	gmtime_s(&tmBuf, &mTime);

	std::strftime(timestampStr, std::size(timestampStr), "%F %TZ",  &tmBuf);

	std::string levelStr;

	switch (mLevel)
	{
	case LogLevel::Error: levelStr = "Error"; break;
	case LogLevel::Warn: levelStr = "Warn"; break;
	default: levelStr = "Info"; break;
	}

	return std::string(timestampStr) + sep + mSection + sep + levelStr + sep + mMessage;
}