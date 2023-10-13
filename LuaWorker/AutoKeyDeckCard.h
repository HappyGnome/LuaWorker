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

#ifndef _AUTO_KEY_DECK_CARD_H_
#define _AUTO_KEY_DECK_CARD_H_

#include <list>

#include "AutoKeyLess.h"
#include "SortedDeck.h"
#include "AutoKey.h"
#include "AutoKeyDeck.h"


namespace AutoKeyCollections
{
	template <typename T_Tag, typename V, class Comp = std::less<V>>
	class Card
	{
		typedef std::shared_ptr<SortedDeck<std::unique_ptr<Card>, less<Card, Card>>> T_HomeDeck;
		typedef std::shared_ptr<AutoKey<T_Tag>> T_AutoKey;

		V mValue;
		T_Tag mTag;
		T_AutoKey mAutoKey;
		T_HomeDeck mHomeDeck;
	public:

		explicit Card(
			const V& value,
			T_AutoKey& autoKey,
			T_HomeDeck& homeDeck)
			: mValue(value),
			mAutoKey(autoKey),
			mHomeDeck(homeDeck)
		{
			if (mAutoKey != nullptr) mTag = mAutoKey->Get();
		}

		explicit Card(
			V&& value,
			T_AutoKey& autoKey,
			T_HomeDeck& homeDeck)
			: mValue(value),
			mAutoKey(autoKey),
			mHomeDeck(homeDeck)
		{
			if (mAutoKey != nullptr) mTag = mAutoKey->Get();
		}

		~Card()
		{
			if (mAutoKey != nullptr)
				mAutoKey->Recycle(mTag);
		}

		V& GetValue()
		{
			return mValue;
		}

		T_Tag GetTag()
		{
			return mTag;
		}

		template<typename T_Tag, typename V, class Comp = std::less<V>>
		friend void ReturnToDeck(std::unique_ptr<Card>&& card)
		{
			if (card == nullptr || card->mHomeDeck == nullptr) return;
			card->mHomeDeck->push(card);
		}

		//Set up so we can use std::less to call the specified Comp class
		friend bool operator<(const Card& a, const Card& b)
		{
			Comp less{};
			return less(a.mValue, b.mValue);
		}

		friend bool operator<(const Card& a, const V& b)
		{
			Comp less{};
			return less(a.mValue, b);
		}

	};
};

#endif