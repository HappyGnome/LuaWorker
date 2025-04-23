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
		
		LuaArgNum(lua_Number val) : mValue(val) {}

		int Unpack(lua_State* pL) const
		{
			lua_pushnumber(pL, mValue);
			return 1;
		}
	};

	/// <summary>
	/// Stack reconstruction step that pushes a bool onto the lua stack
	/// </summary>
	class LuaArgBool : public LuaArgUnpackStep
	{
	private:
		int mValue;
	public:
		
		LuaArgBool(int val) : mValue(val) {}

		int Unpack(lua_State* pL) const
		{
			lua_pushboolean(pL, mValue);
			return 1;
		}
	};

	/// <summary>
	/// Stack reconstruction step that pushes a nil onto the lua stack
	/// </summary>
	class LuaArgNil : public LuaArgUnpackStep
	{
	public:
		
		int Unpack(lua_State* pL) const
		{
			lua_pushnil(pL);
			return 1;
		}
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
		/// <param name="len"></param>
		LuaArgStr(const char* val, size_t len) :mLen(len)
		{
			if (len > 0)
			{
				mValue = new char[len];
				strcpy_s(mValue, len, val);
			}
			else
			{
				mValue = nullptr;
			}
		}

		~LuaArgStr() 
		{
			if (mValue != nullptr)
			{
				delete[] mValue;
			}
		}

		int Unpack(lua_State* pL) const
		{
			if (mValue != nullptr)
			{
				lua_pushlstring(pL, mValue, mLen);
			}
			else
			{
				lua_pushnil(pL);
			}
			return 1;
		}
	};

	/// <summary>
	/// Stack reconstruction step that creates a new table
	/// </summary>
	class LuaArgStartTable : public LuaArgUnpackStep
	{	
	private:
		int mNArr, mNRec;
	public:
		LuaArgStartTable(int narr, int nrec) :mNArr(narr), mNRec(nrec) {}

		/// <summary>
		/// Non-pre-allocating constructor
		/// </summary>
		LuaArgStartTable() :mNArr(0), mNRec(0) {}

		int Unpack(lua_State* pL) const
		{
			lua_createtable(pL, mNArr, mNRec);
			return 1;
		}
	};

	/// <summary>
	/// Stack reconstruction step that sets a table at stack pos -3 with key and value at -2, -1, respectively
	/// </summary>
	class LuaArgSetTable: public LuaArgUnpackStep
	{	
	public:
		int Unpack(lua_State* pL) const
		{
			lua_rawset(pL,-3);
			return -2;
		}
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

	public:

		/// <summary>
		/// Initialise the bundle by taking the top `height` items from the lua stack
		/// Pops height items from the stack
		/// </summary>
		/// <param name="pL"></param>
		/// <param name="height"></param>
		LuaArgBundle(lua_State* pL, int height)
		{
			// TODO add table recursion limit
			int heightDelta = 0;
			int tableDepth = 0;
			bool newTable = false;

			std::unique_ptr<LuaArgUnpackStep> stepToAddValue;
			std::unique_ptr<LuaArgUnpackStep> stepToAddKey;
			
			while (heightDelta < height)
			{
				int luaType = lua_type(pL, -1);

				switch (luaType)
				{
				case LUA_TNUMBER:
					stepToAddValue =std::make_unique<LuaArgNum>(lua_tonumber(pL,-1)); 
					break;
				case LUA_TBOOLEAN:
					stepToAddValue= std::make_unique<LuaArgBool>(lua_toboolean(pL,-1)); 
					break;
				case LUA_TSTRING:
					size_t len;
					const char* str = lua_tolstring(pL, -1, &len);
					stepToAddValue = std::make_unique<LuaArgStr>(str,len); 
					break;

				case LUA_TTABLE:
					lua_pushnil(pL);// to get first key
					tableDepth++;
					heightDelta--;
					newTable = true;

				default: 
					stepToAddValue = std::make_unique<LuaArgNil>(); 
					break;
				}

				if (tableDepth <= 0 && !newTable)
				{
					heightDelta++;
					lua_pop(pL,1);

					mArgs.push_back(stepToAddValue);
					continue;
				}

				// Top of stack is now nil (if newTable), or the key corresponding to value in stepToAdd

				if (!newTable) // Get the key
				{
					bool validKey = true; // All cases but default are success
					luaType = lua_type(pL, -2);

					switch (luaType)
					{
					case LUA_TNUMBER:
						stepToAddKey = std::make_unique<LuaArgNum>(lua_tonumber(pL, -2));
						break;
					case LUA_TBOOLEAN:
						stepToAddKey = std::make_unique<LuaArgBool>(lua_toboolean(pL, -2));
						break;
					case LUA_TSTRING:
						size_t len;
						const char* str = lua_tolstring(pL, -2, &len);
						stepToAddKey = std::make_unique<LuaArgStr>(str, len);
						break;
					default:
						validKey = false;
						break;
					}

					lua_pop(pL, 1); //pop the value only
					heightDelta++;

					if (validKey)
					{
						//Execution order is reversed on unpack: SetTable called with Value-Key-Table on stack.
						mArgs.push_back(std::make_unique<LuaArgSetTable>());
						mArgs.push_back(stepToAddValue);
						mArgs.push_back(stepToAddKey);
					}

				}

				newTable = false; // for next loop

				// Try to pop next key/value from the table
					
				if (lua_next(pL, -2) > 0)
				{	
					heightDelta --; // key popped then key and value added to stack
					continue; // pack & pop value, read the key next pass 
				}
				else
				{

					lua_pop(pL, 1); //pop the table
					heightDelta+=2; // key & table popped
					tableDepth--;

					//End of table (or empty table) - recall execution order is reversed
					mArgs.push_back(std::make_unique<LuaArgStartTable>()); // TODO: Can we get narr and nrec here?
				}
				//TODO: The above is not quite right for nested tables (may need to call next on the parent table too, once child table is read)

			}
		}

		/// <summary>
		/// Reconstruct stored data onto the stack in the given lua state
		/// </summary>
		/// <param name="pL"></param>
		/// <returns>Change in stack height</returns>
		int Unpack(lua_State* pL)
		{
			int height = 0;

			for (auto it = mArgs.crbegin(); it != mArgs.crend(); ++it)
			{
				height += it->Unpack(pL);
			}
			return height;
		}
	};
}

#endif
