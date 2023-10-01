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
 
#pragma once

#ifndef _LOGITEM_H_
#define _LOGITEM_H_

#include<string>
#include<ctime>

namespace LuaWorker
{
	enum class LogLevel
	{
		Error,
		Warn,
		Info
	};

	class LogItem
	{
	private:

		std::string mMessage;
		std::string mSection;
		LogLevel mLevel;
		std::time_t mTime;

	public:

		//---------------------
		// Public methods
		//---------------------

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="level">Log event type</param>
		/// <param name="message">Log item details</param>
		/// <param name="section">Section tag for events logged by this</param>
		explicit LogItem(LogLevel level, std::string message, std::string section);

		/// <summary>
		/// Generate line for log file for this item 
		/// </summary>
		/// <returns>Line for a log file</returns>
		std::string ToString();
	};
};
#endif