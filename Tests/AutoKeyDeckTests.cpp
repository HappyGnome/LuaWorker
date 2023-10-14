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

#include "CppUnitTest.h"
#include "../LuaWorker/AutoKeyDeck.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AutoKeyCollections;

namespace Tests
{

	TEST_CLASS(AutoKeyDeckTests)
	{
	public:
		typedef AutoKeyDeckCard<int, int> T_Card;
		TEST_METHOD(EmptyDeck)
		{
			AutoKeyDeck<int,int,T_Card> deck;
			Assert::IsFalse(deck.pop().has_value());
		}
		/*TEST_METHOD(Pop)
		{
			AutoKeyDeck<int, int> deck;
			deck.push(12);

			auto card = deck.pop();
			Assert::IsTrue(card != nullptr);
			Assert::AreEqual(card->GetValue(),12);
			Assert::AreEqual(card->GetTag(), 0);

			Assert::IsTrue(deck.pop() == nullptr);
		}

		TEST_METHOD(Reorder)
		{
			AutoKeyDeck<int, int> deck(0);
			deck.push(12);
			deck.push(11);
			auto card = deck.pop();
			Assert::AreEqual(card->GetValue(), 11);
			Assert::AreEqual(card->GetTag(), 1);

			card = deck.pop();
			Assert::AreEqual(card->GetValue(), 12);
			Assert::AreEqual(card->GetTag(), 0);

			Assert::IsTrue(deck.pop() == nullptr);
		}

		TEST_METHOD(ConditionalPop)
		{
			AutoKeyDeck<int, int> deck(0);
			deck.push(12);
			deck.push(11);
			auto card = deck.popIfLess(11);
			Assert::IsTrue(card == nullptr);

			card = deck.popIfLess(12);
			Assert::AreEqual(card->GetValue(), 11);

			card = deck.popIfLess(12);
			Assert::IsTrue(card == nullptr);

			deck.pop();
			Assert::IsTrue(deck.pop() == nullptr);
		}*/
	};
}
