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

#ifndef _VALUE_GETTER_H_
#define _VALUE_GETTER_H_
#pragma once

#include <functional>

namespace AutoKeyCollections
{

	template <typename T_ValueIn, typename T_ValueOut = T_ValueIn>
	class ValueGetter
	{
	public:
		typedef T_ValueOut T_Value;

		const T_ValueOut& operator()(const T_ValueIn& in)
		{
			return in.GetValue();
		}

		T_ValueOut& operator()(T_ValueIn& in)
		{
			return in.GetValue();
		}
	};

	template <typename T_ValueIn>
	class ValueGetter<T_ValueIn,T_ValueIn>
	{
	public:
		typedef T_ValueIn T_Value;

		const T_ValueIn& operator()(const T_ValueIn& in)
		{
			return in;
		}

		T_ValueIn& operator()(T_ValueIn& in)
		{
			return in;
		}
	};
};
#endif