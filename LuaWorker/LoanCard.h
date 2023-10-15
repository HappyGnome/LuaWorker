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

#include "AutoKeyLess.h"
#include "SortedDeck.h"
#include "ValueGetter.h"

namespace AutoKeyCollections
{
	template <typename T_Value, class T_Comp = std::less<T_Value>, class T_ValueGet = ValueGetter<T_Value>>
	class LoanCard
	{
	public:
		class Less;
	
	private:
		//Typedefs

		using T_HomeDeck = std::shared_ptr<SortedDeck<LoanCard, typename LoanCard::Less>>;

		T_HomeDeck mHomeDeck;

		T_Value mValue;

	public:

		//Convert value comparisson to Card (and Card-Value) comparisson
		class Less
		{
		public:
			bool operator()(const LoanCard& a, const LoanCard& b)
			{
				T_Comp less{};
				return less(a.mValue, b.mValue);
			}

			bool operator()(const LoanCard& a, const T_Value& b)
			{
				T_Comp less{};
				return less(a.mValue, b);
			}
		};

		//Static


		static void Return(LoanCard&& card)
		{
			if (card.mHomeDeck == nullptr) return;

			card.mHomeDeck->Push(std::move(card));
		}


		//-------------------------------
		// Public methods
		//-------------------------------

		explicit LoanCard()
			: mHomeDeck(), mValue() {}

		explicit LoanCard(const T_HomeDeck& homeDeck, T_Value&& value)
			: mHomeDeck(homeDeck), mValue(value){}

		explicit LoanCard(const T_HomeDeck& homeDeck, const T_Value& value)
			: mHomeDeck(homeDeck), mValue(value) {}

		LoanCard(LoanCard&&) = default;
		LoanCard& operator=(LoanCard&&) = default;


		const typename T_ValueGet::T_Value& GetValue() const
		{
			T_ValueGet get{};
			return get(mValue);
		}

		typename T_ValueGet::T_Value& GetValue()
		{
			T_ValueGet get{};
			return get(mValue);
		}

	};
};

#endif