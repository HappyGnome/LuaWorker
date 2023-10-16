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
	template <typename T_OrderKey, class T_Base>
	class Sortable : public T_Base
	{
	private:
		T_OrderKey mKey;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		Sortable() 
			: T_Base(),mKey() {}

		template<typename ...T_Args>
		explicit Sortable(T_Args&& ...args) 
			: T_Base(std::forward<T_Args>(args)...), mKey() {}

		template<typename ...T_Args>
		explicit Sortable(const T_OrderKey& key, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mKey(key) {}

		template<typename ...T_Args>
		explicit Sortable(T_OrderKey&& key, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mKey(std::move(key)) {}


		void SetSortKey(const T_OrderKey& key) 
		{ 
			mKey = key; 
		}

		void SetSortKey(T_OrderKey&& key) 
		{ 
			mKey = std::move(key); 
		}

		const T_OrderKey& GetSortKey() const
		{
			return mKey;
		}

		template<class T_Comp>
		class Order
		{
		public:
			bool operator()(const Sortable& a, const Sortable& b)
			{
				T_Comp less{};
				return less(a.mKey, b.mKey);
			}

			bool operator()(const Sortable& a, const T_OrderKey& b)
			{
				T_Comp less{};
				return less(a.mKey, b);
			}
		};
		
	};
};
#endif