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

#ifndef _AUTO_KEY_H_
#define _AUTO_KEY_H_
#pragma once

#include <mutex>
#include <vector>
#include <map>

namespace AutoKeyCollections
{
	/// <summary>
	/// Template class to generate and recycle keys/tags
	/// </summary>
	/// <typeparam name="K">Key type</typeparam>
	/// <typeparam name="V">Value type</typeparam>
	template <typename K>
	class AutoKey
	{
	private:

		//-------------------------------
		// Properties
		//-------------------------------

		std::vector<K> mRecycledKeys;
		K mNextNewKey;

	public:

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="k0">Initial key value</param>
		explicit AutoKey(K k0) : mNextNewKey(k0) {}

		/// <summary>
		/// Default constructor
		/// </summary>
		explicit AutoKey() : mNextNewKey() {}

		/// <summary>
		/// Generate a new key, not matching any in the wild
		/// </summary>
		/// <returns>The new key</returns>
		K Get()
		{
			K key;

			if (!mRecycledKeys.empty())
			{
				key = mRecycledKeys.back();
				mRecycledKeys.pop_back();
			}
			else key = mNextNewKey++;

			return key;
		}

		/// <summary>
		/// Return a key for reuse
		/// </summary>
		/// <param name="key">Key to recycle</param>
		void Recycle(K key)
		{
			mRecycledKeys.push_back(key);
		}
	};
};
#endif