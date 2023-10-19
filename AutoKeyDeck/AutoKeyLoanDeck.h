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

#include "LoanDeck.h"
#include "AutoKey.h"
#include "AutoKeyCard.h"
#include "Sortable.h"
#include "SimpleValueR.h"

namespace AutoKeyDeck
{
	namespace Internal
	{
		template<typename T_Value,
			typename T_OrderKey>
		using AutoKeyLoanCard_Sortable = SimpleValueR<T_Value,Sortable<T_OrderKey>>;

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
	/// A LoanDeck with built in autokey and sortable cards for specified value type
	/// </summary>
	/// <typeparam name="T_Value">Value type held by the card</typeparam>
	/// <typeparam name="T_OrderKey">Type used to sort the cards</typeparam>
	/// <typeparam name="T_Tag">Type used to uniquelly* tag each card. *Tags are unique at any time, but may be reused.</typeparam>
	/// <typeparam name="T_Comp">Comparison class used to sort cards by order key</typeparam>
	template <typename T_Value,
		typename T_OrderKey,
		typename T_Tag = std::size_t,
		class T_Comp = std::less<T_OrderKey>>
	class AutoKeyLoanDeck : public LoanDeck<Internal::AutoKeyLoanCard<T_Value,T_OrderKey,T_Tag, T_Comp>, Internal::AutoKeyLoanCard_SortOrder<T_Value, T_OrderKey, T_Comp>>
	{
	private:
		typedef Internal::AutoKeyLoanCard<T_Value, T_OrderKey, T_Tag, T_Comp> T_Card;
		typedef LoanDeck<Internal::AutoKeyLoanCard<T_Value, T_OrderKey, T_Tag, T_Comp>, Internal::AutoKeyLoanCard_SortOrder<T_Value, T_OrderKey, T_Comp>> T_Base;
		typedef AutoKey<T_Tag> T_AutoKey;

		//-------------------------------
		// Properties
		//-------------------------------

		std::shared_ptr<T_AutoKey> mAutoKey;

	public:
		typedef T_Card CardType;

		/// <summary>
		/// Default constructor
		/// </summary>
		AutoKeyLoanDeck() : 
			T_Base(),
			mAutoKey(new AutoKey<T_Tag>())
		{}

		// Hide Make functions - give these more concrete implementations below
		template<typename... T_Args>
		T_Card MakeCard(T_Args&& ...args) = delete;

		// Hide Make functions - give these more concrete implementations below
		template<typename... T_Args>
		void MakeAndKeep(T_Args&& ...args) = delete;

		/// <summary>
		/// Create a new card and return it (the card is not held by the deck).
		/// Arguments moved.
		/// </summary>
		/// <param name="value">Value held by new card</param>
		/// <param name="key">Initial key used for ordering this card</param>
		/// <returns>The new card</returns>
		template<typename T_V, typename T_O = T_OrderKey>
		T_Card MakeCard(T_V&& value, T_O&& key = T_O())
		{
			return T_Base::MakeCard(mAutoKey, std::forward<T_V>(value), std::forward<T_O>(key));
		}

		/// <summary>
		/// Create a new card and add it to the deck.
		/// Arguments moved.
		/// </summary>
		/// <param name="value">Value held by new card</param>
		/// <param name="key">Initial key used for ordering this card</param>
		template<typename T_V = T_Value, typename T_O = T_OrderKey>
		void MakeAndKeep(T_V&& value, T_O&& key = T_O())
		{
			T_Base::MakeAndKeep(mAutoKey, std::forward<T_V>(value), std::forward<T_O>(key));
		}

		template <typename T_Threshold, class T_ThreshComp>
		std::optional<T_Threshold>  GetThreshold() = delete;

		/// <summary>
		/// Get order key of top card
		/// </summary>
		/// <returns></returns>
		std::optional<T_OrderKey>  GetThreshold()
		{
			return T_Base::template GetThreshold<T_OrderKey, Internal::AutoKeyLoanCard_SortOrder<T_Value, T_OrderKey, T_Comp>>();
		}
	};
};
#endif