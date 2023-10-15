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

#ifndef _SORTABLE_H_
#define _SORTABLE_H_
#pragma once

#include <functional>

namespace AutoKeyCollections
{
	/// <summary>
	/// Wrapper for SortedDeck which tags each contained object with a key. Keys may be reused but no two objects will hold the same key at once.
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename T_Value, typename T_OrderKey = std::size_t>
	class Sortable
	{
	private:

		T_Value mValue;
		T_OrderKey mKey;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		explicit Sortable() 
			: mValue(), mKey() {}

		explicit Sortable(const T_Value& value) 
			: mValue(value), mKey() {}
		explicit Sortable(const T_OrderKey& key) 
			: mValue(), mKey(key) {}
		explicit Sortable(const T_Value& value, const T_OrderKey& key) 
			: mValue(value), mKey(key) {}

		explicit Sortable(T_Value&& value) 
			: mValue(std::move(value)), mKey() {}
		explicit Sortable(T_OrderKey&& key) 
			: mValue(), mKey(std::move(key)) {}
		explicit Sortable(T_Value&& value,T_OrderKey&& key) 
			: mValue(std::move(value)), mKey(std::move(key)) {}

		void SetValue(const T_Value& value) 
		{ 
			mValue = value; 
		}

		void SetSortKey(const T_OrderKey& key) 
		{ 
			mKey = key; 
		}

		void SetValue(T_Value&& value) 
		{ 
			mValue = std::move(value); 
		}

		void SetSortKey(T_OrderKey&& key) 
		{ 
			mKey = std::move(key); 
		}

		const T_Value& GetValue() const
		{
			return mValue;
		}

		const T_Value& GetSortKey() const
		{
			return mValue;
		}

		T_Value& GetValue()
		{
			return mValue;
		}

		T_Value& GetSortKey()
		{
			return mValue;
		}

		template<class T_Comp>
		class Order
		{
			bool operator()(const Sortable& a, const Sortable& b)
			{
				T_Comp less{};
				return less(a.mKey, b.mKey);
			}
		};
		
	};
};
#endif