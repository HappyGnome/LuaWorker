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

#ifndef _AUTO_KEY_DECK_CARD_H_
#define _AUTO_KEY_DECK_CARD_H_
#pragma once

#include <list>

#include "AutoKey.h"


namespace AutoKeyCollections
{
	template <typename T_Value, typename T_Tag>
	class AutoKeyCard
	{
		typedef std::shared_ptr <AutoKey<T_Tag>> T_AutoKey;

	private:
		T_Tag mTag;
		T_AutoKey mAutoKey;

		T_Value mValue;

		void InitTag()
		{
			if (mAutoKey != nullptr) mTag = mAutoKey->Get();
		}

	public:

		template <class T_Comp>
		class Less
		{
		public:
			bool operator()(const AutoKeyCard& a, const AutoKeyCard& b)
			{
				T_Comp less{};
				return less(a.mValue, b.mValue);
			}
		};

		AutoKeyCard() : mAutoKey(),mTag(){}

		explicit AutoKeyCard(const T_AutoKey& autoKey, T_Value&& value)
			: mAutoKey(autoKey),
			mTag()
		{
			InitTag();
		}

		explicit AutoKeyCard(const T_AutoKey& autoKey, const T_Value& value)
			: mAutoKey(autoKey),
			mTag()
		{
			InitTag();
		}

		AutoKeyCard(AutoKeyCard&&) = default;
		AutoKeyCard& operator=(AutoKeyCard&&) = default;

		/// <summary>
		/// Copy constructor
		/// Not copyable (Tags must remain unique)
		/// </summary>
		/// <param name=""></param>
		AutoKeyCard(const AutoKeyCard&) = delete;
		AutoKeyCard& operator=(const AutoKeyCard&) = delete;

		~AutoKeyCard()
		{
			try
			{
				if (mAutoKey != nullptr)
					mAutoKey->Recycle(mTag);
			}
			catch (const std::exception&)
			{
				// Suppress non-critical exceptions in destructor
			}
		}

		T_Tag GetTag()
		{
			return mTag;
		}

		const T_Value& GetValue() const
		{
			return mValue;
		}

		T_Value& GetValue()
		{
			return mValue;

		}
		/*const typename T_ValueGet::T_Value& GetValue() const
		{
			T_ValueGet get{};
			return get(mValue);
		}

		typename T_ValueGet::T_Value& GetValue()
		{
			T_ValueGet get{};
			return get(mValue);
		}*/ //TODO
	};
};

#endif