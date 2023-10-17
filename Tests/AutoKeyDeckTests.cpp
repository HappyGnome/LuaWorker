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
			decltype(deck)::CardType res;

			Assert::IsFalse(deck.Pop(res));
		}
		
		TEST_METHOD(Pop)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			decltype(deck)::CardType res;

			deck.MakeAndKeep(12);

			Assert::IsTrue(deck.Pop(res));
			Assert::AreEqual(12, res.GetValue());
			Assert::AreEqual(0, res.GetSortKey());

			Assert::IsFalse(deck.Pop(res));
		}
		
		TEST_METHOD(Reorder)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			decltype(deck)::CardType res;

			deck.MakeAndKeep(12, 1);
			deck.MakeAndKeep(11, 0);

			deck.Pop(res);
			Assert::AreEqual(11, res.GetValue());
			Assert::AreEqual(1, res.GetTag());
			Assert::AreEqual(0, res.GetSortKey());

			deck.Pop(res);
			Assert::AreEqual(12, res.GetValue());
			Assert::AreEqual(0, res.GetTag());
			Assert::AreEqual(1, res.GetSortKey());

			Assert::IsFalse(deck.Pop());
		}

		TEST_METHOD(ConditionalPop)
		{
			AutoKeyLoanDeck<char, int, int> deck;

			decltype(deck)::CardType res;

			deck.MakeAndKeep('b', 12);
			deck.MakeAndKeep('a', 11);

			
			Assert::IsFalse(deck.PopIfLess(11));
			Assert::IsFalse(deck.PopIfLess(11,res));

			Assert::IsTrue(deck.PopIfLess(12,res));
			Assert::AreEqual('a', res.GetValue());

			Assert::IsFalse(deck.PopIfLess(12));

			Assert::IsTrue(deck.Pop());
			Assert::IsFalse(deck.Pop());
		}

		TEST_METHOD(CustomComp)
		{
			AutoKeyLoanDeck<std::string, float, char, std::greater_equal<float>> deck;

			decltype(deck)::CardType res;

			res = deck.MakeCard(std::string("World"), 1.0f);
			res.Return(std::move(res));

			res = deck.MakeCard(std::string("Hello"), 1.7f);
			res.Return(std::move(res));

			res = decltype(res)();

			deck.Pop(res);
			Assert::AreEqual(std::string("Hello"), res.GetValue());
			Assert::AreEqual(char(1), res.GetTag());
			Assert::AreEqual(1.7f, res.GetSortKey());

			deck.Pop(res);
			Assert::AreEqual(std::string("World"), res.GetValue());
			Assert::AreEqual(char(0), res.GetTag());
			Assert::AreEqual(1.0f, res.GetSortKey());

			Assert::IsFalse(deck.Pop());
		}

	};
}
