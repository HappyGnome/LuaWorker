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

#include <string>

#include "CppUnitTest.h"
#include "AutoKeyLoanDeck.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AutoKeyDeck;

namespace Tests
{

	TEST_CLASS(AutoKeyDeckTests)
	{
	public:
		TEST_METHOD(EmptyDeck)
		{
			AutoKeyLoanDeck<int,int,int> deck;

			Assert::IsFalse(deck.Pop().has_value());
		}
		
		TEST_METHOD(Pop)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			std::optional<decltype(deck)::CardType> card;

			deck.MakeAndKeep(12);

			card = deck.Pop();
			Assert::IsTrue(card.has_value());
			Assert::AreEqual(12, card.value().GetValue());
			Assert::AreEqual(0, card.value().GetSortKey());

			Assert::IsFalse(deck.Pop().has_value());
		}
		
		TEST_METHOD(Reorder)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			std::optional<decltype(deck)::CardType> card;

			deck.MakeAndKeep(12, 1);
			deck.MakeAndKeep(11, 0);

			Assert::AreEqual(0, deck.GetThreshold().value());

			card = deck.Pop();
			Assert::AreEqual(11, card.value().GetValue());
			Assert::AreEqual(1, card.value().GetTag());
			Assert::AreEqual(0, card.value().GetSortKey());

			Assert::AreEqual(1, deck.GetThreshold().value());

			card = deck.Pop();
			Assert::AreEqual(12, card.value().GetValue());
			Assert::AreEqual(0, card.value().GetTag());
			Assert::AreEqual(1, card.value().GetSortKey());

			Assert::IsFalse(deck.Pop().has_value());
		}

		TEST_METHOD(ConditionalPop)
		{
			AutoKeyLoanDeck<char, int, int> deck;

			std::optional<decltype(deck)::CardType> card;

			deck.MakeAndKeep('b', 12);
			deck.MakeAndKeep('a', 11);

			
			Assert::IsFalse(deck.PopIfLess(11).has_value());

			card = deck.PopIfLess(12);
			Assert::IsTrue(card.has_value());
			Assert::AreEqual('a', card.value().GetValue());

			Assert::IsFalse(deck.PopIfLess(12).has_value());

			Assert::IsTrue(deck.Pop().has_value());
			Assert::IsFalse(deck.Pop().has_value());
		}

		TEST_METHOD(CustomComp)
		{
			AutoKeyLoanDeck<std::string, float, char, std::greater_equal<float>> deck;

			decltype(deck)::CardType res;
			std::optional<decltype(deck)::CardType> card;

			res = deck.MakeCard(std::string("World"), 1.0f);
			res.Return(std::move(res));

			res = deck.MakeCard(std::string("Hello"), 1.7f);
			res.Return(std::move(res));

			res = decltype(res)();

			card = deck.Pop();
			Assert::AreEqual(std::string("Hello"), card.value().GetValue());
			Assert::AreEqual(char(1), card.value().GetTag());
			Assert::AreEqual(1.7f, card.value().GetSortKey());

			card = deck.Pop();
			Assert::AreEqual(std::string("World"), card.value().GetValue());
			Assert::AreEqual(char(0), card.value().GetTag());
			Assert::AreEqual(1.0f, card.value().GetSortKey());

			Assert::IsFalse(deck.Pop().has_value());
		}

	};
}
