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
#include "Empty.h"

namespace AutoKeyDeck
{
	/// <summary>
	/// Chainable template class, adding a sort order via member keys.
	/// </summary>
	/// <typeparam name="T_Base">Chain base class</typeparam>
	/// <typeparam name="T_OrderKey"></typeparam>
	template <typename T_OrderKey, class T_Base = Empty>
	class Sortable : public T_Base
	{
	private:
		T_OrderKey mKey;

	public:

		/// <summary>
		/// Default constructor
		/// </summary>
		Sortable() 
			: T_Base(),mKey() {}

		/// <summary>
		/// Chainable constructor with default order key
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="...args">Args for base class</param>
		template<typename ...T_Args>
		explicit Sortable(T_Args&& ...args) 
			: T_Base(std::forward<T_Args>(args)...), mKey() {}

		/// <summary>
		/// Chainable constructor setting order key
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="key">Sort key</param>
		/// <param name="...args">Args for base class</param>
		template<typename T_O = T_OrderKey, typename ...T_Args>
		explicit Sortable(T_O&& key, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mKey(std::forward<T_O>(key)) {}

		/// <summary>
		/// (Re)assign sort key
		/// </summary>
		/// <param name="key"></param>
		template<typename T_O = T_OrderKey>
		void SetSortKey(T_O&& key) 
		{ 
			mKey = std::forward<T_O>(key);
		}

		/// <summary>
		/// Returns current sort key
		/// </summary>
		/// <returns></returns>
		const T_OrderKey& GetSortKey() const
		{
			return mKey;
		}

		/// <summary>
		/// Sample implementation of an ordering class, compatible with popIfLess functionality of LoanDeck
		/// </summary>
		template<class T_Comp>
		class Order
		{
		public:
			/// <summary>
			/// Compares sort cards directly
			/// </summary>
			bool operator()(const Sortable& a, const Sortable& b)
			{
				T_Comp less{};
				return less(a.mKey, b.mKey);
			}

			/// <summary>
			/// Compares a sort card with a raw orderkey
			/// </summary>
			bool operator()(const Sortable& a, const T_OrderKey& b)
			{
				T_Comp less{};
				return less(a.mKey, b);
			}
		};
		
	};
};
#endif