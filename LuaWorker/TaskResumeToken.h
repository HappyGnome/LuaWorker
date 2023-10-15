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
	template<typename T, typename T_Duration = float, typename T_Clock = std::chrono::system_clock>
	class TaskResumeToken {
	private:
		std::chrono::time_point<T_Clock> mResumeAt;
		T mTaskHandle;

	public:
		TaskResumeToken() :mTaskHandle(0){};

		explicit TaskResumeToken(const std::chrono::time_point<T_Clock>& resumeAt, const T& handle) : mTaskHandle(handle), mResumeAt(resumeAt) {}

		explicit TaskResumeToken(const std::chrono::duration<T_Duration>& resumeIn, const T& handle) : mTaskHandle(handle)
		{
			mResumeAt = T_Clock::now() + std::chrono::duration_cast<T_Clock::duration>(resumeIn);
		}

		const T& GetHandle() const
		{
			return mTaskHandle;
		}

		const std::chrono::time_point<T_Clock>& GetResumeAt() const
		{
			return mResumeAt;
		}

		void Reschedule(const std::chrono::time_point<T_Clock>& resumeAt) const
		{
			mResumeAt = resumeAt;
		}

		void Reschedule(const std::chrono::duration<T_Duration>& resumeIn)
		{
			mResumeAt = T_Clock::now() + std::chrono::duration_cast<T_Clock::duration>(resumeIn);
		}

	};
}

#endif