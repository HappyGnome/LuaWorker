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

#ifndef _CHRONO_KEY_H_
#define _CHRONO_KEY_H_

#include <chrono>

namespace LuaWorker
{
	template<typename T>
	class ChronoKey {
	private:
		std::chrono::time_point mKey;
		T mTieBreak

	public:
		explicit ChronoKey(const std::chrono::time_point& key, const T& tiebreak) : mTKey(key), mTieBreak(tiebreak) {}

		template<typename S>
		friend bool operator<(const ChronoKey<S>& A const ChronoKey<S>& B)
		{
			return A.mKey < B.mKey || (A.mKey == B.mKey && A.mTieBreak < B.mTieBreak);
		}

		template<typename S>
		friend bool operator<(const ChronoKey<S>& A const std::chrono::time_point& B)
		{
			return A.mKey < B;
		}
	};
}

#endif