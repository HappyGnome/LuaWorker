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

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


namespace LuaWorker
{
	/// <summary>
	/// Base class for stack reconstruction steps
	/// </summary>
	class LuaArgUnpackStep
	{
	public:
		/// <summary>
		/// Unpack this arg onto the stack of a given lua state 
		/// </summary>
		/// <param name="pL"></param>
		/// <returns>The change in the stack height caused by this operation</returns>
		virtual int Unpack(lua_State* pL) const = 0;
	};

	/// <summary>
	/// Stack reconstruction step that pushes a number onto the lua stack
	/// </summary>
	class LuaArgNum : public LuaArgUnpackStep
	{
	private:
		lua_Number mValue;
	public:
		
		LuaArgNum(lua_Number val);

		int Unpack(lua_State* pL) const;
	};

	/// <summary>
	/// Stack reconstruction step that pushes a bool onto the lua stack
	/// </summary>
	class LuaArgBool : public LuaArgUnpackStep
	{
	private:
		int mValue;
	public:
		
		LuaArgBool(int val);

		int Unpack(lua_State* pL) const;
	};

	///// <summary>
	///// Stack reconstruction step that pops values from the stack
	///// </summary>
	//class LuaArgPop : public LuaArgUnpackStep
	//{
	//public:
	//	
	//	LuaArgPop() {}

	//	int Unpack(lua_State* pL) const
	//	{
	//		lua_pop(pL, 1);
	//		return -1;
	//	}
	//};


	/// <summary>
	/// Stack reconstruction step that pushes a nil onto the lua stack
	/// </summary>
	class LuaArgNil : public LuaArgUnpackStep
	{
	public:
		
		int Unpack(lua_State* pL) const;
	};

	/// <summary>
	/// Stack reconstruction step that pushes a lua string onto the lua stack
	/// </summary>
	class LuaArgStr : public LuaArgUnpackStep
	{
	private:
		 char* mValue;
		 size_t mLen;
	public:
		
		/// <summary>
		/// Makes a copy of a raw string value. The string may contain nulls throughout. 
		/// </summary>
		/// <param name="val"></param>
		/// <param name="len">Length of the string (excluding null terminator)</param>
		LuaArgStr(const char* val, size_t len);

		~LuaArgStr();
		
		int Unpack(lua_State* pL) const;
	};

	/// <summary>
	/// Stack reconstruction step that creates a new table
	/// </summary>
	class LuaArgStartTable : public LuaArgUnpackStep
	{	
	private:
		int mNArr, mNRec;
	public:
		LuaArgStartTable(int narr, int nrec);

		/// <summary>
		/// Non-pre-allocating constructor
		/// </summary>
		LuaArgStartTable();

		int Unpack(lua_State* pL) const;
	};

	/// <summary>
	/// Stack reconstruction step that sets a table at stack pos -3 with key and value at -2, -1, respectively
	/// </summary>
	class LuaArgSetTable: public LuaArgUnpackStep
	{	
	public:
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
		std::vector<std::unique_ptr<LuaArgUnpackStep>> mArgs;

		/// <summary>
		/// Assumes top of the stack is a table
		/// Add instructions to mArgs to recreate that table (when executed in reverse order).
		/// After execution, the table is popped.
		/// </summary>
		/// <param name="pL"></param>
		void BundleTable(lua_State* pL);

	public:

		/// <summary>
		/// Initialise the bundle by taking the top `height` items from the lua stack
		/// Pops height items from the stack
		/// </summary>
		/// <param name="pL"></param>
		/// <param name="height"></param>
		LuaArgBundle(lua_State* pL, int height);

		/// <summary>
		/// Reconstruct stored data onto the stack in the given lua state
		/// </summary>
		/// <param name="pL"></param>
		/// <returns>Change in stack height</returns>
		int Unpack(lua_State* pL);
	};
}

#endif
