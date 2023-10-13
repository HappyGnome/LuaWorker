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
#include "../LuaWorker/SortedDeck.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AutoKeyCollections;

namespace Tests
{
	template<typename T>
	class SillyWrapper
	{
		T mValue;
	public:
		SillyWrapper() = default;
		SillyWrapper(const T& val) : mValue(val){}

		T GetValue() { return mValue; }

		template<typename S>
		friend bool operator<(const SillyWrapper<S>& a, const SillyWrapper<S>& b)
		{
			return a.mValue < b.mValue;
		}

		template<typename S>
		friend bool operator<(const SillyWrapper<S>& a, const S& b)
		{
			return a.mValue < b;
		}
	};

	/*class less
	{
	public:

		template<typename S>
		bool operator()(const SillyWrapper<S>& a, const S& b)
		{
			return a < b;
		}
	};*/

	TEST_CLASS(SortedDeckTests)
	{
	public:

		TEST_METHOD(EmptyDeck)
		{
			SortedDeck<int> deck;

			int result;
			Assert::IsFalse(deck.pop(result));

		}

		TEST_METHOD(BasicOrdering)
		{
			SortedDeck<int> deck;

			deck.push(43);
			deck.push(45);
			deck.push(1);

			int result;
			Assert::IsTrue(deck.pop(result));
			Assert::AreEqual(result, 1);

			Assert::IsTrue(deck.pop(result));
			Assert::AreEqual(result, 43);

			Assert::IsTrue(deck.pop(result));
			Assert::AreEqual(result, 45);

			Assert::IsFalse(deck.pop(result));
		}

		TEST_METHOD(ConditionalPop)
		{
			SortedDeck<SillyWrapper<int>> deck;

			deck.push(SillyWrapper <int>(43));
			deck.push(SillyWrapper <int>(45));
			deck.push(SillyWrapper <int>(1));

			SillyWrapper <int> result;
			Assert::IsFalse(deck.popIfLess(1, result));

			Assert::IsFalse(deck.popIfLess(SillyWrapper <int>(1), result));

			Assert::IsTrue(deck.popIfLess(2, result));
			Assert::AreEqual(result.GetValue(), 1);

			Assert::IsTrue(deck.popIfLess(SillyWrapper <int>(44), result));
			Assert::AreEqual(result.GetValue(), 43);

			Assert::IsTrue(deck.popIfLess(SillyWrapper <int>(46), result));
			Assert::AreEqual(result.GetValue(), 45);

			Assert::IsFalse(deck.pop(result));
		}
	};
}
