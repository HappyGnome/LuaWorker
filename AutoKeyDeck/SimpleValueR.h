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

#ifndef _SIMPLE_VALUE_R_H_
#define _SIMPLE_VALUE_R_H_
#pragma once

#include <functional>

#include "Empty.h"

namespace AutoKeyDeck
{

	/// <summary>
	/// Chainable template class for read-only wrapper of a specified value type.
	/// </summary>
	/// <typeparam name="T_Value">Value type wrapped</typeparam>
	/// <typeparam name="T_Base">Base class in chain</typeparam>
	template <typename T_Value, class T_Base = Empty>
	class SimpleValueR : public T_Base
	{
	private:
		T_Value mValue;
	public:

		/// <summary>
		/// Default constructor
		/// </summary>
		SimpleValueR()
			: T_Base(), mValue() {}

		/// <summary>
		/// Constructor initializing wrapped value (copied).
		/// </summary>
		/// <param name="value"></param>
		explicit SimpleValueR(const T_Value& value)
			: T_Base(), mValue(value) {}

		/// <summary>
		/// Constructor initializing wrapped value (moved).
		/// </summary>
		/// <param name="value"></param>
		explicit SimpleValueR(T_Value&& value)
			: T_Base(), mValue(std::move(value)) {}

		/// <summary>
		/// Chainable constructor, initializing wrapped value (copied)
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="value"></param>
		/// <param name="...args"></param>
		template<typename ...T_Args>
		explicit SimpleValueR(const T_Value& value, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mValue(value) {}

		/// <summary>
		/// Chainable constructor, initializing wrapped value (moved)
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="value"></param>
		/// <param name="...args"></param>
		template<typename ...T_Args>
		explicit SimpleValueR(T_Value&& value, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mValue(std::move(value)) {}


		/// <summary>
		/// Fetch wrapped value
		/// </summary>
		/// <returns></returns>
		const T_Value& GetValue() const
		{
			return mValue;
		}

		/// <summary>
		/// Fetch wrapped value
		/// </summary>
		/// <returns></returns>
		T_Value& GetValue()
		{
			return mValue;
		}

	};

};
#endif