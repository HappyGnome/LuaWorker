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

#ifndef _DECK_CARD_H_
#define _DECK_CARD_H_

#include <list>

#include "AutoKeyLess.h"
#include "SortedDeck.h"


namespace AutoKeyCollections
{
	template <typename T_OrderKey, class T_Comp = std::less<T_OrderKey>>
	class DeckCard
	{
		typedef std::shared_ptr<SortedDeck<DeckCard, std::less<DeckCard>>> T_HomeDeck;

		T_OrderKey mOrderKey;
		T_HomeDeck mHomeDeck;
	public:

		explicit DeckCard(const T_HomeDeck& homeDeck)
			: mHomeDeck(homeDeck), mOrderKey(){}

		void SetOrderKey(const T_OrderKey& newKey) 
		{
			mOrderKey = newKey;
		}

		template<typename T_Tag, typename V, class Comp = std::less<V>>
		friend void ReturnToDeck(DeckCard&& card)
		{
			card.mHomeDeck->push(std::move(card));
		}

		//Set up so we can use std::less to call the specified Comp class
		friend bool operator<(const DeckCard& a, const DeckCard& b)
		{
			T_Comp less{};
			return less(a.mValue, b.mValue);
		}

		friend bool operator<(const DeckCard& a, const T_OrderKey& b)
		{
			T_Comp less{};
			return less(a.mValue, b);
		}

	};
};

#endif