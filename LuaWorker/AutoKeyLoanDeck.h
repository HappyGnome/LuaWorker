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
#include "Sortable.h"
#include "SimpleValueR.h"

namespace AutoKeyCollections
{
	namespace Internal
	{
		template<typename T_Value,
			typename T_OrderKey>
		using AutoKeyLoanCard_Sortable = Sortable<T_OrderKey, SimpleValueR_Base<T_Value>>;

		template<typename T_Value,
			typename T_OrderKey,
			class T_Comp>
		using AutoKeyLoanCard_SortOrder = typename AutoKeyLoanCard_Sortable<T_Value, T_OrderKey>::template Order<T_Comp>;

		template<typename T_Value,
			typename T_OrderKey,
			typename T_Tag>
		using AutoKeyLoanCard_AutoKeyCard = AutoKeyCard<T_Tag, AutoKeyLoanCard_Sortable<T_Value, T_OrderKey>>;

		template<typename T_Value,
			typename T_OrderKey,
			typename T_Tag,
			class T_Comp>
		using AutoKeyLoanCard = LoanCard<AutoKeyLoanCard_SortOrder<T_Value, T_OrderKey, T_Comp>, AutoKeyLoanCard_AutoKeyCard<T_Value, T_OrderKey, T_Tag>>;
	};

	/// <summary>
	/// Wrapper for SortedDeck which tags each contained object with a key. Keys may be reused but no two objects will hold the same key at once.
	/// </summary>
	/// <typeparam name="V">Value type</typeparam>
	/// <typeparam name="Comp">Key comparator</typeparam>
	template <typename T_Value,
		typename T_OrderKey,
		typename T_Tag = std::size_t,
		class T_Comp = std::less<T_OrderKey>>
	class AutoKeyLoanDeck : public LoanDeck<Internal::AutoKeyLoanCard<T_Value,T_OrderKey,T_Tag, T_Comp>, Internal::AutoKeyLoanCard_SortOrder<T_Value, T_OrderKey, T_Comp>>
	{
	private:
		typedef Internal::AutoKeyLoanCard<T_Value, T_OrderKey, T_Tag, T_Comp> T_Card;

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
			return LoanDeck::MakeCard(mAutoKey, std::forward <T_Args>(args)...);
		}

		template <typename ...T_Args>
		void MakeAndKeep(T_Args&& ...args)
		{
			LoanDeck::MakeAndKeep(mAutoKey, std::forward <T_Args>(args)...);
		}
	};
};
#endif