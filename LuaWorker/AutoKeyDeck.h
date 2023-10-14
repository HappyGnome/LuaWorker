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

#ifndef _AUTO_KEY_DECK_H_
#define _AUTO_KEY_DECK_H_

#include<list>
#include<optional>

#include "AutoKeyLess.h"
#include "SortedDeck.h"
#include "AutoKey.h"
#include "AutoKeyDeckCard.h"

namespace AutoKeyCollections
{
	/// <summary>
	/// Wrapper for SortedDeck which tags each contained object with a key. Keys may be reused but no two objects will hold the same key at once.
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename T_Tag, typename T_OrderKey, typename T_Card, class T_Comp = std::less<T_OrderKey>>
	class AutoKeyDeck
	{
	private:

		static_assert(std::is_base_of<AutoKeyDeckCard<T_Tag, T_OrderKey, T_Comp>, T_Card>::value);

		typedef SortedDeck<T_Card, std::less<T_Card>> T_Deck;
		typedef AutoKey<T_Tag> T_AutoKey;

		//-------------------------------
		// Properties
		//-------------------------------

		std::shared_ptr<T_AutoKey> mAutoKey;
		std::shared_ptr<T_Deck> mDeck;
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		explicit AutoKeyDeck() : 
			mAutoKey(new AutoKey<T_Tag>()),
			mDeck(new T_Deck())
		{}

		T_Card&& push()
		{
			return T_Card(mDeck, mAutoKey);
		}

		std::optional<T_Card> pop()
		{
			return mDeck->pop();
		}

		template <typename T_Threshold, class T_ThreshComp = less<Card, T_Threshold>>
		std::optional<T_Card>  popIfLess(const T_Threshold& thresh)
		{
			return mDeck->popIfLess<T_Threshold, T_ThreshComp>(thresh);
		}
	};
};
#endif