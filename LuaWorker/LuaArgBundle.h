/*****************************************************************************\
*
*  Copyright 2025 HappyGnome
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

#ifndef _LUAARGBUNDLE_H_
#define _LUAARGBUNDLE_H_
#pragma once

//#include <iostream>
//#include <filesystem>
//#include <deque> 
#include<vector>
#include<memory>
#include<cassert>
#include<string>
#include<variant>

#ifdef _BENCHMARK_OBJ_COUNTERS_
#include <atomic>
#endif

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


namespace LuaWorker
{
	enum LuaArgStepType
	{
		Nil,
		Number,
		Bool,
		String,
		TableStart,
		TableSet
	};

	/// <summary>
	/// Base class for stack reconstruction steps
	/// </summary>
	class LuaArgUnpackStep
	{
	private:
		LuaArgStepType mType;

		struct LTableSpec
		{
			int NArr, NRec;
		};

		std::variant<	
			lua_Number,
			int,
			std::string,
			LTableSpec>
		 mData;

	public:

		/// <summary>
		/// Construct step to push a number
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static LuaArgUnpackStep PushNumber(lua_Number value);

		/// <summary>
		/// Construct step that pushes nil
		/// </summary>
		/// <returns></returns>
		static LuaArgUnpackStep PushNil();

		/// <summary>
		/// Construct step that pushes a boolean
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static LuaArgUnpackStep PushBool(int value);

		/// <summary>
		/// Construct a step that pushes a string
		/// </summary>
		/// <param name="value"></param>
		/// <param name="len"></param>
		/// <returns></returns>
		static LuaArgUnpackStep PushString(const char* value, size_t len);

		/// <summary>
		/// Construct a step that sets a table value
		/// </summary>
		/// <returns></returns>
		static LuaArgUnpackStep SetTable();

		/// <summary>
		/// Construct a step that starts a table (with pre-allocation)
		/// </summary>
		/// <param name="nArr"></param>
		/// <param name="nRec"></param>
		/// <returns></returns>
		static LuaArgUnpackStep StartTable(int nArr, int nRec);

		/// <summary>
		/// Unpack this arg onto the stack of a given lua state 
		/// </summary>
		/// <param name="pL"></param>
		/// <returns>The change in the stack height caused by this operation</returns>
		int Unpack(lua_State* pL) const;
	};


	//---------------

	/// <summary>
	/// LuaArgBundle implements methods to copy lua values from the stack in one lua state and 
	/// write them back to the stack later in the same or a different lua state. 
	/// 
	/// Valid types that can be bundled are: strings, numbers, booleans, and tables (with string or numeric keys, and values of Valid type) 
	/// </summary>
	class LuaArgBundle 
	{
	private:
		std::vector<LuaArgUnpackStep> mArgs;

		/// <summary>
		/// Assumes top of the stack is a table
		/// Add instructions to mArgs to recreate that table (when executed in reverse order).
		/// After execution, the table is popped.
		/// </summary>
		/// <param name="pL"></param>
		/// <param name = "maxTableDepth">maximum levels of the table heirarchy to allow. E.g. if maxDepth == 0, tables are packed as nils. If it's 1, tables witih are top level table are ignored, and so on.</param>
		void BundleTable(lua_State* pL, int maxTableDepth);

	public:

		static constexpr int DefaultMaxTableDepth = 10;

		//------------------------------
		// Diagnostic statics
		//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		static std::atomic<int> CountPushed;
		static std::atomic<int> CountDeleted;
		static std::atomic<int> PeakCount ;
#endif

		/// <summary>
		/// Initialise the bundle by taking the top `height` items from the lua stack
		/// Pops height items from the stack
		/// </summary>
		/// <param name="pL"></param>
		/// <param name="height"></param>
		/// <param name = "maxTableDepth">maximum levels of the table heirarchy to allow. E.g. if maxDepth == 0, tables are packed as nils. If it's 1, tables witih are top level table are ignored, and so on.</param>
		explicit LuaArgBundle(lua_State* pL, int height, int maxTableDepth );

		/// <summary>
		/// Initialize an empty arg bundle
		/// </summary>
		LuaArgBundle();

#ifdef _BENCHMARK_OBJ_COUNTERS_
		~LuaArgBundle();

		LuaArgBundle(const LuaArgBundle&) = delete;
		LuaArgBundle& operator=(const LuaArgBundle&) = delete;

		LuaArgBundle(LuaArgBundle&&) = default;
		LuaArgBundle& operator=(LuaArgBundle&&) = default;
#endif

		/// <summary>
		/// Reconstruct stored data onto the stack in the given lua state
		/// </summary>
		/// <param name="pL"></param>
		/// <returns>Change in stack height</returns>
		int Unpack(lua_State* pL);
	};
}

#endif
