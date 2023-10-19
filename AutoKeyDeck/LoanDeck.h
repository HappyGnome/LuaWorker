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

#ifndef _LOAN_DECK_H_
#define _LOAN_DECK_H_
#pragma once

#include<list>
#include<optional>

#include "SortedDeck.h"
#include "LoanCard.h"
//#include "SimpleValueR.h"

namespace AutoKeyDeck
{
	/// <summary>
	/// Wrapper for SortedDeck and factory for LoanCards linked to it.
	/// </summary>
	/// <typeparam name="T_Card">Card type</typeparam>
	/// <typeparam name="T_Comp">Card comparison class</typeparam>
	template <typename T_Card,
			  class T_Comp>
	class LoanDeck
	{
	private:

		using T_Deck = SortedDeck<T_Card, T_Comp>;

		//-------------------------------
		// Properties
		//-------------------------------

		std::shared_ptr<T_Deck> mDeck;
	public:

		//-------------------------------
		// Public methods
		//-------------------------------

		/// <summary>
		/// Default constructor
		/// </summary>
		LoanDeck() :
			mDeck(new T_Deck())
		{}

		/// <summary>
		/// Construct a card with this a home deck and return it to caller.
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="...args">Arguments for the card constructor</param>
		/// <returns>The new card</returns>
		template<typename... T_Args>
		T_Card MakeCard(T_Args&& ...args)
		{			
			return T_Card(mDeck, std::forward <T_Args>(args)...);
		}

		/// <summary>
		/// Construct a card with this a home deck and add it to the deck.
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="...args">Arguments for the card constructor</param>
		template<typename... T_Args>
		void MakeAndKeep(T_Args&& ...args)
		{
			T_Card::Return(T_Card(mDeck, std::forward <T_Args>(args)...));
		}

		/// <summary>
		/// Try to remove top card of the deck.
		/// </summary>
		/// <param name="out">Set to the card popped on success</param>
		/// <returns>True if a card was removed from the deck</returns>
		bool Pop(T_Card& out)
		{
			return mDeck->Pop(out);
		}

		/// <summary>
		/// Try to remove top card of the deck
		/// </summary>
		/// <returns>True if a card was removed from the deck</returns>
		bool Pop()
		{
			return mDeck->Pop();
		}

		/// <summary>
		/// Conditionally pop the top card of the deck
		/// </summary>
		/// <typeparam name="T_Threshold"></typeparam>
		/// <typeparam name="T_ThreshComp">Comparison class (see std::less)</typeparam>
		/// <param name="thresh">Value to which the top card is compared</param>
		/// <param name="out">Receives popped value</param>
		/// <returns>True if card removed from deck</returns>
		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool  PopIfLess(T_Threshold&& thresh, T_Card& out)
		{
			return mDeck->PopIfLess<T_Threshold, T_ThreshComp>(std::forward<T_Threshold>(thresh), out);
		}

		/// <summary>
		/// Conditionally pop the top card of the deck
		/// </summary>
		/// <typeparam name="T_Threshold"></typeparam>
		/// <typeparam name="T_ThreshComp">Comparison class (see std::less)</typeparam>
		/// <param name="thresh">Value to which the top card is compared</param>
		/// <returns>True if card removed from deck</returns>
		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool  PopIfLess(T_Threshold&& thresh)
		{
			return mDeck->PopIfLess<T_Threshold, T_ThreshComp>(std::forward<T_Threshold>(thresh));
		}

		/// <summary>
		/// Get order key of top card
		/// </summary>
		/// <returns></returns>
		template <typename T_Threshold, class T_ThreshComp>
		T_Threshold  GetThreshold()
		{
			return mDeck->GetThreshold<T_Threshold, T_ThreshComp>();
		}
	};
};
#endif