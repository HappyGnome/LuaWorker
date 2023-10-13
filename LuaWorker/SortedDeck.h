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

#ifndef _SORTED_DECK_H_
#define _SORTED_DECK_H_

#include<list>

#include "AutoKeyLess.h"


namespace AutoKeyCollections
{
	/// <summary>
	/// Simple template class to maintain a sorted list, allowing removal at the front, and random inserts
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename V, class Comp = std::less<V>>
	class SortedDeck
	{
	private:

		//-------------------------------
		// Properties
		//-------------------------------

		std::list<V> mDeck;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		SortedDeck() {}

		/// <summary>
		/// Add an item to the collection
		/// </summary>
		/// <param name="value">value to store</param>
		void push(V&& value)
		{
			Comp less = Comp{};

			for (auto it = mDeck.begin(); it != mDeck.end(); ++it) 
			{
				if (less(value, *it))
				{
					mDeck.insert(it, std::move(value));
					return;
				}
			}

			mDeck.push_back(std::move(value));
		}

		/// <summary>
		/// Remove first item (out parameter receives the value)
		/// </summary>
		/// <param name="out">set to value popped if 'true' returned</param>
		/// <returns>True if and only if item found</returns>
		bool pop(V &out)
		{
			if (mDeck.empty()) return false;
			out = std::move(mDeck.front());
			mDeck.pop_front();
			return true;
		}

		/// <summary>
		/// Pop the first element of the deck, if it is less that a comparable value threshold
		/// </summary>
		/// <typeparam name="T_Threshold">Type of threshold</typeparam>
		/// <typeparam name="T_ThreshComp">CClass implementing () operator for comparisson</typeparam>
		/// <param name="thresh">Cuttoff</param>
		/// <param name="out">Value popped</param>
		/// <returns>True if value popped</returns>
		template <typename T_Threshold, class T_ThreshComp = less<V, T_Threshold>>
		bool popIfLess(const T_Threshold& thresh, V& out)
		{
			T_ThreshComp less{};

			if (!mDeck.empty() && less(mDeck.front(), thresh))
			{
				out = std::move(mDeck.front());
				mDeck.pop_front();
				return true;
			}
			return false;
		}
	};
};
#endif