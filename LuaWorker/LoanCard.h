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

#ifndef _LOAN_CARD_H_
#define _LOAN_CARD_H_
#pragma once

#include <list>
#include <memory>

#include "SortedDeck.h"
#include "SimpleValueR.h"
#include "Empty.h"

namespace AutoKeyCollections
{
	/// <summary>
	/// Chainable template class with a linked home LoanDeck, to which instances can be returned
	/// </summary>
	/// <typeparam name="T_Base">Base class in the chain</typeparam>
	/// <typeparam name="T_Comp">Class whose operator() compares instances</typeparam>
	template <class T_Comp,class T_Base = Empty>
	class LoanCard : public T_Base
	{	
	private:

		//Typedefs
		using T_HomeDeck = std::shared_ptr<SortedDeck<LoanCard, T_Comp>>;

		//-------------------------------
		// Properties
		//-------------------------------

		T_HomeDeck mHomeDeck;

	public:

		//-------------------------------
		// Static methods
		//-------------------------------

		/// <summary>
		/// Move a card back to its home deck
		/// </summary>
		/// <param name="card">Card to move</param>
		static void Return(LoanCard&& card)
		{
			if (card.mHomeDeck == nullptr) return;

			card.mHomeDeck->Push(std::move(card));
		}


		//-------------------------------
		// Public methods
		//-------------------------------

		/// <summary>
		/// Default constructor
		/// </summary>
		LoanCard()
			: T_Base(), mHomeDeck() {}

		/// <summary>
		/// Chainable constructor, setting the instance home deck
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="homeDeck"></param>
		/// <param name="...args"></param>
		template<typename ...T_Args>
		explicit LoanCard(const T_HomeDeck& homeDeck,T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mHomeDeck(homeDeck){}

	};
};

#endif