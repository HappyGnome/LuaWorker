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

#ifndef _LOGSECTION_H_
#define _LOGSECTION_H_

#include<string>
#include<memory>

#include "LogItem.h"
#include "LogStack.h"
#include "LogLevel.h"

namespace LuaWorker
{
	class LogSection
	{
	private:

		std::shared_ptr<LogStack> mLog;

		std::string mSection;

	public:

		/// <summary>
		/// Construct log section
		/// </summary>
		/// <param name="stack">Logger to receive messages</param>
		/// <param name="section">Section name for logs pushed by this instance</param>
		explicit LogSection(std::shared_ptr<LogStack> stack, std::string section);

		/// <summary>
		/// Copy Constructor
		/// </summary>
		/// <param name="other"></param>
		LogSection(const LogSection& other);

		/// <summary>
		/// Default destructor
		/// </summary>
		~LogSection() = default;

		/// <summary>
		/// Copy assignment
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		LogSection& operator= (const LogSection& other);

		/// <summary>
		/// Push log message to queue
		/// </summary>
		/// <param name="level">Message type/level</param>
		/// <param name="message">Message content</param>
		void Push(const LogLevel& level, const std::string& message);

		/// <summary>
		/// Push error message for exception
		/// </summary>
		/// <param name="e">Exception from which to generate log</param>
		void Push(const std::exception& e);
	};
}

#endif