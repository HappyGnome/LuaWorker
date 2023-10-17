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
#include "Empty.h"

namespace AutoKeyCollections
{
	/// <summary>
	/// Chainable template class, holding a tag from an autokey. Automatically returning the key upon destruction.
	/// </summary>
	/// <typeparam name="T_Base">Base class in chain</typeparam>
	/// <typeparam name="T_Tag">Tag type</typeparam>
	template <typename T_Tag, class T_Base = Empty>
	class AutoKeyCard : public T_Base
	{
		typedef std::shared_ptr <AutoKey<T_Tag>> T_AutoKey;

	private:
		T_Tag mTag;
		T_AutoKey mAutoKey;

		/// <summary>
		/// Get tag from linked autokey
		/// </summary>
		void InitTag()
		{
			if (mAutoKey != nullptr) mTag = mAutoKey->Get();
		}

	public:

		/// <summary>
		/// Default constructor
		/// </summary>
		AutoKeyCard() : mAutoKey(),mTag(){}

		/// <summary>
		/// Chained fully-initializing constructor
		/// </summary>
		/// <typeparam name="...T_Args"></typeparam>
		/// <param name="autoKey">Home autokey</param>
		/// <param name="...args">Args for base class</param>
		template <typename ...T_Args>
		explicit AutoKeyCard(const T_AutoKey& autoKey, T_Args&& ...args)
			: T_Base(std::forward<T_Args>(args)...), mAutoKey(autoKey)
		{
			InitTag();
		}

		/// <summary>
		/// Move constructor
		/// </summary>
		/// <param name=""></param>
		AutoKeyCard(AutoKeyCard&&) = default;

		/// <summary>
		/// Move assignment
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		AutoKeyCard& operator=(AutoKeyCard&&) = default;

		/// <summary>
		/// Copy constructor deleted
		/// Not copyable (Tags must remain unique)
		/// </summary>
		/// <param name=""></param>
		AutoKeyCard(const AutoKeyCard&) = delete;

		/// <summary>
		/// Copy assignment deleted
		/// Not copyable (Tags must remain unique)
		/// </summary>
		/// <param name=""></param>
		AutoKeyCard& operator=(const AutoKeyCard&) = delete;

		/// <summary>
		/// Destructor - return key to autokey
		/// </summary>
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

		/// <summary>
		/// Tag getter
		/// </summary>
		/// <returns>Tag assigned to this</returns>
		T_Tag GetTag()
		{
			return mTag;
		}
	};
};

#endif