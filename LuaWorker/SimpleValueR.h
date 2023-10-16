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

namespace AutoKeyCollections
{

	/// <summary>
	/// Trivial read only class, to add the GetValue method to a template class
	/// </summary>
	/// <typeparam name="T_Value"></typeparam>
	template <typename T_Value, class T_Base>
	class SimpleValueR : public T_Base
	{
	private:
		T_Value mValue;
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		explicit SimpleValueR()
			: T_Base(), mValue() {}

		explicit SimpleValueR(const T_Value& value)
			: T_Base(), mValue(value) {}

		explicit SimpleValueR(T_Value&& value)
			: T_Base(), mValue(std::move(value)) {}

		template<typename ...T_Args>
		explicit SimpleValueR(const T_Value& value, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mValue(value) {}

		template<typename ...T_Args>
		explicit SimpleValueR(T_Value&& value, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mValue(std::move(value)) {}


		const T_Value& GetValue() const
		{
			return mValue;
		}

		T_Value& GetValue()
		{
			return mValue;
		}

	};

	/// <summary>
	/// Trivial read only class, to add the GetValue method to a template class
	/// </summary>
	/// <typeparam name="T_Value"></typeparam>
	template <typename T_Value>
	class SimpleValueR_Base
	{
	private:
		T_Value mValue;
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		explicit SimpleValueR_Base()
			: mValue() {}

		explicit SimpleValueR_Base(const T_Value& value)
			: mValue(value) {}

		explicit SimpleValueR_Base(T_Value&& value)
			: mValue(std::move(value)){}


		const T_Value& GetValue() const
		{
			return mValue;
		}

		T_Value& GetValue()
		{
			return mValue;
		}

	};

};
#endif