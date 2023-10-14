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
#include "DeckCard.h"


namespace AutoKeyCollections
{
	template <typename T_Tag, typename T_OrderKey, class T_Comp = std::less<T_OrderKey>>
	class AutoKeyDeckCard : public DeckCard<T_OrderKey,T_Comp>
	{
		typedef AutoKey<T_Tag> T_AutoKey;
		typedef std::shared_ptr<SortedDeck<AutoKeyDeckCard, std::less<AutoKeyDeckCard>>> T_HomeDeck;
		//typedef DeckCard<T_OrderKey, T_Comp> T_DeckCard;

		T_Tag mTag;
		std::shared_ptr<T_AutoKey> mAutoKey;
		T_HomeDeck mHomeDeck;
	public:

		explicit AutoKeyDeckCard(
			const T_HomeDeck& homeDeck, T_AutoKey& autoKey)
			: DeckCard(homeDeck),
			mAutoKey(autoKey)
		{
			if (mAutoKey != nullptr) mTag = mAutoKey->Get();
		}

		~AutoKeyDeckCard()
		{
			try
			{
				if (mAutoKey != nullptr)
					mAutoKey->Recycle(mTag);
			}
			catch (const std::exception&)
			{
				// Suppress non-critical exceptions in destructor
			}
		}

		T_Tag GetTag()
		{
			return mTag;
		}
	};
};

#endif