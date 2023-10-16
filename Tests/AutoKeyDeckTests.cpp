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
#include "../LuaWorker/AutoKeyLoanDeck.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AutoKeyCollections;

namespace Tests
{
	
	/*class SimpleCard : public AutoKeyDeckCard<int, int, SimpleCard, int>
	{
		using T_HomeDeck = DeckCard::T_HomeDeck;
		using T_AutoKey = AutoKeyDeckCard::T_AutoKey;

		int NewInChild;
	public:
		using AutoKeyDeckCard::AutoKeyDeckCard;

		//SimpleCard(SimpleCard&& other) : AutoKeyDeckCard(std::move(other))
		//{
		//	std::cout << "Test";
		//}
		//SimpleCard& operator=(SimpleCard&& other) = default;

		int GetVal() { return mValue; }
	};*/

	TEST_CLASS(AutoKeyDeckTests)
	{
	public:
		TEST_METHOD(EmptyDeck)
		{
			AutoKeyLoanDeck<int,int,int> deck;
			AutoKeyLoanDeck<int, int, int>::CardType res;

			Assert::IsFalse(deck.Pop(res));
		}
		
		TEST_METHOD(Pop)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			AutoKeyLoanDeck<int, int, int>::CardType res;

			deck.MakeAndKeep(0,12);

			Assert::IsTrue(deck.Pop(res));
			Assert::AreEqual(12, res.GetValue());
			//Assert::AreEqual(card.value().GetValue().GetTag(), 0);

			Assert::IsFalse(deck.Pop(res));
		}
		
		TEST_METHOD(Reorder)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			AutoKeyLoanDeck<int, int, int>::CardType res;

			deck.MakeAndKeep(1,12);//TODO reverse key and value in constructor
			deck.MakeAndKeep(0,11);

			deck.Pop(res);
			Assert::AreEqual(11, res.GetValue());
			Assert::AreEqual(1, res.GetTag());

			deck.Pop(res);
			Assert::AreEqual(12, res.GetValue());
			Assert::AreEqual(0, res.GetTag());

			Assert::IsFalse(deck.Pop());
		}

		TEST_METHOD(ConditionalPop)
		{
			AutoKeyLoanDeck<int, int, int> deck;

			AutoKeyLoanDeck<int, int, int>::CardType res;

			deck.MakeAndKeep(12,99);
			deck.MakeAndKeep(11,-7);

			
			Assert::IsFalse(deck.PopIfLess(11));
			Assert::IsFalse(deck.PopIfLess(11,res));

			Assert::IsTrue(deck.PopIfLess(12,res));
			Assert::AreEqual(-7, res.GetValue());

			Assert::IsFalse(deck.PopIfLess(12));

			Assert::IsTrue(deck.Pop());
			Assert::IsFalse(deck.Pop());
		}
	};
}
