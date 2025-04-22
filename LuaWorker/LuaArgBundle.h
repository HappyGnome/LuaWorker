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

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


namespace LuaWorker
{
// TODO: split into files
	class LuaArgUnpackStep
	{
	public:
		/// <summary>
		/// Unpack this arg onto the stack of a given lua state 
		/// </summary>
		/// <param name="pL"></param>
		virtual void Unpack(lua_State* pL) = 0;
	};

	class LuaArgNum : public LuaArgUnpackStep
	{
	private:
		lua_Number mValue;
	public:
		// TODO setters
		void Unpack(lua_State* pL) 
		{
			lua_pushnumber(pL, mValue);
		}
	};

	// TODO: String and bool


	class LuaArgStartTable : public LuaArgUnpackStep
	{	
	private:
		int mNArr, mNRec;
	public:
		// TODO setters
		void Unpack(lua_State* pL) 
		{
			lua_createtable(pL,mNArr,mNRec);
		}
	};


	class LuaArgSetTable: public LuaArgUnpackStep
	{	
	public:
		// TODO setters
		void Unpack(lua_State* pL) 
		{
			lua_rawset(pL,-3);
		}
	};



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

		// TODO List builder
		// TODO Unpack all
	};
}

#endif
