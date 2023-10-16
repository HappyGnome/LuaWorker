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

#include "AutoKeyLess.h"
#include "SortedDeck.h"
#include "LoanCard.h"
//#include "SimpleValueR.h"

namespace AutoKeyCollections
{
	/// <summary>
	/// Wrapper for SortedDeck which tags each contained object with a key. Keys may be reused but no two objects will hold the same key at once.
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
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

		/// <summary>
		/// Constructor
		/// </summary>
		explicit LoanDeck() :
			mDeck(new T_Deck())
		{}

		template<typename... T_Args>
		T_Card MakeCard(T_Args&& ...args)
		{			
			return T_Card(mDeck, std::forward <T_Args>(args)...);
		}

		template<typename... T_Args>
		void MakeAndKeep(T_Args&& ...args)
		{
			T_Card::Return(T_Card(mDeck, std::forward <T_Args>(args)...));
		}

		bool Pop(T_Card& out)
		{
			return mDeck->Pop(out);
		}

		bool Pop()
		{
			return mDeck->Pop();
		}

		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool  PopIfLess(const T_Threshold& thresh, T_Card& out)
		{
			return mDeck->PopIfLess<T_Threshold, T_ThreshComp>(thresh, out);
		}

		template <typename T_Threshold, class T_ThreshComp = T_Comp>
		bool  PopIfLess(const T_Threshold& thresh)
		{
			return mDeck->PopIfLess<T_Threshold, T_ThreshComp>(thresh);
		}
	};
};
#endif