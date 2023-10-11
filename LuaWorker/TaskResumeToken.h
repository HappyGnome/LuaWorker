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

#ifndef _TASK_RESUME_TOKEN_H_
#define _TASK_RESUME_TOKEN_H_

#include <chrono>

namespace LuaWorker
{
	template<typename T>
	class TaskResumeToken {
	private:
		std::chrono::time_point<float> mResumeAt;
		T mTaskHandle;

	public:
		TaskResumeToken(const std::chrono::time_point<float>& resumeAt, const T& handle) : mTaskHandle(handle), mResumeAt(resume) {}

		TaskResumeToken(const std::chrono::duration<float>& resumeIn, const T& handle) : mTaskHandle(handle)
		{
			mResumeAt = std::chrono::system_clock::now() + resumeIn;
		}

		const T& GetHandle() const
		{
			return mTaskHandle;
		}

		const std::chrono::time_point<float>& GetResumeAt() const
		{
			return mResumeAt;
		}

		void Reschedule(const std::chrono::time_point<float>& resumeAt) const
		{
			mResumeAt = resumeAt;
		}

		void Reschedule(const std::chrono::duration<float>& resumeIn) const
		{
			mResumeAt = std::chrono::system_clock::now() + resumeIn;
		}

	};
}

#endif