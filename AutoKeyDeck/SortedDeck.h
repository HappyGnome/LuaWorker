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
		template<typename T_V = T_Value>
		void Push(T_V&& value)
		{
			T_Comp less = T_Comp{};

			for (auto it = mDeck.begin(); it != mDeck.end(); ++it) 
			{
				if (less(value, *it))
				{
					mDeck.insert(it, std::forward<T_V>(value));
					return;
				}
			}

			mDeck.push_back(std::forward<T_V>(value));
		}

		/// <summary>
		/// Remove first item (out parameter receives the value)
		/// </summary>
		/// <returns>The popped value, or empty if none popped</returns>
		std::optional<T_Value> Pop()
		{
			if (mDeck.empty()) return std::optional<T_Value>();

			std::optional<T_Value> out(std::move(mDeck.front()));
			mDeck.pop_front();

			return out;
		}


		/// <summary>
		/// Pop the first element of the deck, if it is less that a comparable value threshold
		/// </summary>
		/// <typeparam name="T_Threshold">Type of threshold</typeparam>
		/// <typeparam name="T_ThreshComp">Class implementing () operator for comparison</typeparam>
		/// <param name="thresh">Cuttoff</param>
		/// <returns>The popped value, or empty if none popped</returns>
		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		std::optional<T_Value> PopIfLess(T_Threshold&& thresh)
		{
			T_ThreshComp less{};

			if (mDeck.empty() || !less(mDeck.front(), std::forward<T_Threshold>(thresh))) 
				return std::optional<T_Value>();
			
			std::optional<T_Value> out(std::move(mDeck.front()));
			mDeck.pop_front();

			return out;
		}

		/// <summary>
		/// Get value above which PopIfLess would return an item
		/// </summary>
		/// <typeparam name="T_Threshold">Type of threshold</typeparam>
		/// <typeparam name="T_ThreshComp">Class implementing () operator, mapping T_Value to T_Threshold</typeparam>
		/// <returns>Threshold value of top value</returns>
		template <typename T_Threshold, class T_ThreshComp>
		std::optional<T_Threshold> GetThreshold()
		{
			if (mDeck.empty()) return std::optional<T_Threshold>();

			T_ThreshComp comp{};

			return std::make_optional<T_Threshold>(comp(mDeck.front()));

		}
	};
};
#endif