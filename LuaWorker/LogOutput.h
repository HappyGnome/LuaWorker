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

#ifndef _LOG_OUTPUT_H_
#define _LOG_OUTPUT_H_

#include<string>

#include "LogLevel.h"

namespace LuaWorker
{
	class LogOutput
	{

	public:

		/// <summary>
		/// Try to get next log line, removing that line from the log stack.
		/// </summary>
		/// <param name="message">Log line output</param>
		/// <param name="message">Log level output</param>
		/// <returns>True unless the list is empty</returns>
		virtual bool PopLine(std::string& message, LogLevel& level) = 0;

		/// <summary>
		/// Try to get next log line, removing that line from the log stack.
		/// </summary>
		/// <param name="message">Log line output</param>
		/// <returns>True unless the list is empty</returns>
		virtual bool PopLine(std::string& message) = 0;
	};
}
#endif