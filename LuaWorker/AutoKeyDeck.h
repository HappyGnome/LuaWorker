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
	template <typename T_Tag, typename V, class Comp = std::less<V>>
	class AutoKeyDeck
	{
	private:

		typedef Card<T_Tag, V, Comp> Card;
		typedef SortedDeck<std::unique_ptr<Card>, less<Card, Card>> T_Deck;
		//-------------------------------
		// Properties
		//-------------------------------

		std::shared_ptr<AutoKey<T_Tag>> mAutoKey;
		std::shared_ptr<T_Deck> mDeck;
	public:

		/// <summary>
		/// Constructor
		/// </summary>
		explicit AutoKeyDeck(const T_Tag& k0) : 
			mAutoKey(new AutoKey<T_Tag>(k0)),
			mDeck(new T_Deck())
		{}

		void push(V&& value)
		{
			mDeck -> push(std::make_unique<Card>(value, mAutoKey, mDeck));
		}

		std::unique_ptr<Card> pop()
		{
			std::unique_ptr<Card> out;
			if (mDeck->pop(out)) return out;
			return nullptr;
		}

		template <typename T_Threshold, class T_ThreshComp = less<Card, T_Threshold>>
		std::unique_ptr<Card>  popIfLess(const T_Threshold& thresh)
		{
			std::unique_ptr<Card> out;
			if (mDeck->popIfLess<T_Threshold, T_ThreshComp>(thresh,out)) return out;
			return nullptr;
		}
	};
};
#endif