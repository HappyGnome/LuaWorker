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

#ifndef _AUTO_KEY_DECK_H_
#define _AUTO_KEY_DECK_H_
#pragma once

#include<list>
#include<optional>

#include "AutoKeyLess.h"
#include "LoanDeck.h"
#include "AutoKey.h"
#include "AutoKeyCard.h"

namespace AutoKeyCollections
{

	/// <summary>
	/// Wrapper for SortedDeck which tags each contained object with a key. Keys may be reused but no two objects will hold the same key at once.
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename T_Value,
		typename T_Tag = std::size_t,
		class T_Comp = std::less<T_Value>, 
		class T_Card = LoanCard<AutoKeyCard<T_Value, T_Tag>, typename AutoKeyCard<T_Value, T_Tag>::Less<T_Comp>/*, ValueGetter<AutoKeyCard<T_Value, T_Tag>, T_Value> //TODO */ >>

	class AutoKeyLoanDeck : public LoanDeck<AutoKeyCard<T_Value, T_Tag>, AutoKeyCard<T_Value, T_Tag>::Less<T_Comp>, T_Card>
	{
	private:

		typedef AutoKey<T_Tag> T_AutoKey;

		//-------------------------------
		// Properties
		//-------------------------------

		std::shared_ptr<T_AutoKey> mAutoKey;

	public:
		typedef T_Card CardType;

		/// <summary>
		/// Constructor
		/// </summary>
		AutoKeyLoanDeck() : 
			LoanDeck(),
			mAutoKey(new AutoKey<T_Tag>())
		{}

		template <typename ...T_Args>
		T_Card MakeCard(T_Args&& ...args)
		{
			return LoanDeck::MakeCard(mAutoKey, T_Value(std::forward <T_Args>(args)...));
		}
	};
};
#endif