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

#pragma once

#ifndef _AUTO_KEY_MAP_H_
#define _AUTO_KEY_MAP_H_

//#include <iostream>
//#include <filesystem>
//#include <thread>
//#include <mutex>
//#include <string>
#include <vector>
#include <map>

//#include "Task.h"

extern "C" {
//#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
}
//using namespace std::chrono_literals;

namespace LuaWorker
{
	/// <summary>
	/// Simple template class to push and pop a collection of objects 
	/// using keys that are generated and recycled automatically
	/// </summary>
	/// <typeparam name="K">Key type</typeparam>
	/// <typeparam name="V">Value type</typeparam>
	template <typename K, class V>
	class AutoKeyMap
	{
	private:

		//-------------------------------
		// Properties
		//-------------------------------

		std::map<K, std::shared_ptr<V>> mStore;
		std::vector<K> mRecycledKeys;
		K mNextNewKey;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="k0">Initial key value</param>
		explicit AutoKeyMap(K k0) : mNextNewKey(k0) {}

		/// <summary>
		/// Add an item to the collection
		/// </summary>
		/// <param name="value">value to store (wrapped in a shared_ptr)</param>
		/// <returns>The key for retrieving value later</returns>
		K push (std::shared_ptr<V> value) 
		{
			K key = mNextNewKey;

			if (!mRecycledKeys.empty())
			{
				key = mRecycledKeys.back();
				mRecycledKeys.pop_back();
			}
			else mNextNewKey++;

			mStore[key] = value;

			return key;
		}

		/// <summary>
		/// Get item at specified key
		/// </summary>
		/// <param name="key">Key to fetch</param>
		/// <returns>Stored value (if found)</returns>
		/// <exception>std::out_of_range if key invalid</exception>
		std::shared_ptr<V> at (K key)
		{
			return mStore.at(key);
		}

		/// <summary>
		/// Remove item at specified key, and return it
		/// </summary>
		/// <param name="key">Key to fetch</param>
		/// <returns>Stored value (if found)</returns>
		/// <exception>std::out_of_range if key invalid</exception>
		std::shared_ptr<V> pop (K key)
		{
			std::shared_ptr<V> v = mStore.at(key);
			mStore.erase(key);

			mRecycledKeys.push_back(key);

			return v;
		}
	};
};
#endif