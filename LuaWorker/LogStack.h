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

#ifndef _LOGSTACKBASE_H_
#define _LOGSTACKBASE_H_
#pragma once

#include<deque>
#include<string>
#include<mutex>

#include "LogItem.h"
#include "LogLevel.h"
#include "LogOutput.h"

namespace LuaWorker
{
	class LogStack : public LogOutput
	{
	private:

		std::deque<LogItem> mMessages;

		std::mutex mMessagesMutex;

		std::size_t mCapacity;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="capacity">Max number of log lines to store</param>
		explicit LogStack(std::size_t capacity = 100);

		/// <summary>
		/// Try to get next log line, removing that line from the log stack.
		/// </summary>
		/// <param name="message">Log line output</param>
		/// <param name="message">Log level output</param>
		/// <returns>True unless the list is empty</returns>
		virtual bool PopLine(std::string& message, LogLevel &level) override;

		/// <summary>
		/// Try to get next log line, removing that line from the log stack.
		/// </summary>
		/// <param name="message">Log line output</param>
		/// <returns>True unless the list is empty</returns>
		virtual bool PopLine(std::string& message) override;

		/// <summary>
		/// Add log item to the current list
		/// </summary>
		/// <param name="message">Log item ot add</param>
		void Push(const LogItem& message);
	};
}
#endif