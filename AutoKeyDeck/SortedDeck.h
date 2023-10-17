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

#ifndef _DECK_H_
#define _DECK_H_
#pragma once

#include<list>
#include<optional>

namespace AutoKeyDeck
{
	/// <summary>
	/// Simple template class to maintain a sorted list, allowing removal at the front, and random inserts
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename T_Value, class T_Comp = std::less<T_Value>>
	class SortedDeck
	{
	private:

		//-------------------------------
		// Properties
		//-------------------------------

		std::list<T_Value> mDeck;

	public:

		/// <summary>
		/// Default constructor
		/// </summary>
		SortedDeck() {}

		/// <summary>
		/// Add an item to the collection
		/// </summary>
		/// <param name="value">value to store</param>
		void Push(T_Value&& value)
		{
			T_Comp less = T_Comp{};

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
		/// <returns>True if and only if item found and removed</returns>
		bool Pop(T_Value& out)
		{
			if (mDeck.empty()) return false;

			out = std::move(mDeck.front());
			mDeck.pop_front();

			return true;
		}

		/// <summary>
		/// Remove first item 
		/// </summary>
		/// <returns>True if and only if item found and removed</returns>
		bool Pop()
		{
			if (mDeck.empty()) return false;
			mDeck.pop_front();
			return true;
		}

		/// <summary>
		/// Pop the first element of the deck, if it is less that a comparable value threshold
		/// </summary>
		/// <typeparam name="T_Threshold">Type of threshold</typeparam>
		/// <typeparam name="T_ThreshComp">Class implementing () operator for comparison</typeparam>
		/// <param name="thresh">Cuttoff</param>
		/// <param name="out">Value popped</param>
		/// <returns>True if value popped</returns>
		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool PopIfLess(const T_Threshold& thresh, T_Value& out)
		{
			T_ThreshComp less{};

			if (mDeck.empty() || !less(mDeck.front(), thresh)) return false;
			
			return Pop(out);
		}

		/// <summary>
		/// Pop the first element of the deck, if it is less that a comparable value threshold
		/// </summary>
		/// <typeparam name="T_Threshold">Type of threshold</typeparam>
		/// <typeparam name="T_ThreshComp">Class implementing () operator for comparison</typeparam>
		/// <param name="thresh">Cuttoff</param>
		/// <returns>True if value popped</returns>
		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool PopIfLess(const T_Threshold& thresh)
		{
			T_ThreshComp less{};

			if (mDeck.empty() || !less(mDeck.front(), thresh)) return false;

			return Pop();
		}
	};
};
#endif