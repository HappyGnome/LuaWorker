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

#include "LuaArgBundle.h"

using namespace LuaWorker;

//LuaArgNum

LuaArgNum::LuaArgNum(lua_Number val) : mValue(val) {}

int LuaArgNum::Unpack(lua_State* pL) const
{
	lua_pushnumber(pL, mValue);
	return 1;
}

// LuaArgBool

LuaArgBool::LuaArgBool(int val) : mValue(val) {}

int LuaArgBool::Unpack(lua_State* pL) const
{
	lua_pushboolean(pL, mValue);
	return 1;
}

// LuaArgNil	
int LuaArgNil::Unpack(lua_State* pL) const
{
	lua_pushnil(pL);
	return 1;
}

// LuaArgStr

LuaArgStr::LuaArgStr(const char* val, size_t len) :mLen(len)
{
	mValue = new char[len+1];
	memcpy_s(mValue, len+1, val,len+1);

	//------------------------------
	// Diagnostic statics
	//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
	LuaArgStr::CountPushed++;
	if (LuaArgStr::CountPushed > LuaArgStr::PeakStrArgCount + LuaArgStr::CountDeleted) LuaArgStr::PeakStrArgCount = LuaArgStr::CountPushed - LuaArgStr::CountDeleted;
#endif
}

LuaArgStr::~LuaArgStr() 
{
	if (mValue != nullptr)
	{
		delete[] mValue;

		//------------------------------
		// Diagnostic statics
		//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		LuaArgStr::CountDeleted++;
#endif
	}
}

int LuaArgStr::Unpack(lua_State* pL) const
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

//------------------------------
// Diagnostic statics
//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		std::atomic<int> LuaArgStr::CountPushed = 0;
		std::atomic<int> LuaArgStr::CountDeleted = 0;
		std::atomic<int> LuaArgStr::PeakStrArgCount = 0;
#endif

// LuaArgStartTable

LuaArgStartTable::LuaArgStartTable(int narr, int nrec) :mNArr(narr), mNRec(nrec) {}

LuaArgStartTable::LuaArgStartTable() :mNArr(0), mNRec(0) {}

int LuaArgStartTable::Unpack(lua_State* pL) const
{
	lua_createtable(pL, mNArr, mNRec);
	return 1;
}

// LuaArgSetTable

int LuaArgSetTable::Unpack(lua_State* pL) const
{
	lua_rawset(pL,-3);
	return -2;
}


//---------------
// LuaArgBundle

/// <summary>
/// Helper struct for bundling tables, to track table and sub-table sizes
/// </summary>
struct TableSizeCounter
{
	int nArr = 0;
	int nRec = 0;
};

void LuaArgBundle::BundleTable(lua_State* pL, int maxTableDepth)
{
	if (maxTableDepth <= 0)
	{
		mArgs.push_back(std::make_unique<LuaArgNil>()); 
		return;
	}

	std::vector<TableSizeCounter> parentSizes(LuaArgBundle::DefaultMaxTableDepth);

	parentSizes[1].nArr = (int)lua_objlen(pL, -1); //table depth is an index into this. Initially 1.

	int tableDepth = 1;
	bool skipKey = true; // Key is nil (start of new table), or otherwise does not correspond to a key-value pair when this bundle is unpacked

	lua_pushnil(pL); // Get first key-value pair on lua_next
	
	size_t len;
	const char* str;

	// Each iteration starts with stack <Top>|Key|Table
	while (tableDepth > 0)
	{
		std::unique_ptr<LuaArgUnpackStep> stepToAddValue = nullptr;
		std::unique_ptr<LuaArgUnpackStep> stepToAddKey = nullptr;

		if (!skipKey)
		{
			switch (lua_type(pL, -1))
			{
			case LUA_TNUMBER:
				stepToAddKey = std::make_unique<LuaArgNum>(lua_tonumber(pL, -1));
				break;
			case LUA_TBOOLEAN:
				stepToAddKey = std::make_unique<LuaArgBool>(lua_toboolean(pL, -1));
				break;
			case LUA_TSTRING:
				str = lua_tolstring(pL, -1, &len);
				stepToAddKey = std::make_unique<LuaArgStr>(str, len);
				break;
			default:
				assert(false); // skipKey should have been set, and instructions to create the corresponding value not added if this key is invalid. 
				stepToAddKey = nullptr; // Prevent C26800 in the std::move below
			}
			
			mArgs.push_back(std::move(stepToAddKey));
		}

		skipKey = false;

		if (lua_next(pL, -2) == 0)
		{
			TableSizeCounter tsc = parentSizes[tableDepth];
			mArgs.push_back(std::make_unique<LuaArgStartTable>(tsc.nArr, std::max(0,tsc.nRec - tsc.nArr)));//TODO: Test

			tableDepth--;

			lua_pop(pL, 1);// pop table, now it's read

			continue;
		}
		//key-value pair now at top of stack

		//Check key is supported type, or we skip to the next
		switch (lua_type(pL, -2))
		{
		case LUA_TNUMBER: break;
		case LUA_TBOOLEAN: break;
		case LUA_TSTRING: break;
		default:
			lua_pop(pL, 1); //pop value
			skipKey = true;
			continue;
		}

		mArgs.push_back(std::make_unique<LuaArgSetTable>());
		parentSizes[tableDepth].nRec++;

		switch (lua_type(pL, -1)) // value type
		{
		case LUA_TNUMBER:
			stepToAddValue = std::make_unique<LuaArgNum>(lua_tonumber(pL, -1));
			break;
		case LUA_TBOOLEAN:
			stepToAddValue = std::make_unique<LuaArgBool>(lua_toboolean(pL, -1));
			break;
		case LUA_TSTRING:
			str = lua_tolstring(pL, -1, &len);
			stepToAddValue = std::make_unique<LuaArgStr>(str, len);
			break;

		case LUA_TTABLE:
			if (tableDepth >= maxTableDepth)
			{
				stepToAddValue = std::make_unique<LuaArgNil>();
				break;
			}
			else
			{
				tableDepth++;
				if (parentSizes.size() <= tableDepth) parentSizes.emplace_back();
				parentSizes[tableDepth].nRec = 0;
				parentSizes[tableDepth].nArr = (int)lua_objlen(pL, -1);

				lua_pushnil(pL);// to get first key of new table
				skipKey = true;
				continue;
			}
		default: 
			stepToAddValue = std::make_unique<LuaArgNil>(); 
			break;
		}

		mArgs.push_back(std::move(stepToAddValue));
		lua_pop(pL, 1); // Pop the value
	}
}

/// <summary>
/// Default constructor
/// </summary>
LuaArgBundle::LuaArgBundle()
{

#ifdef _BENCHMARK_OBJ_COUNTERS_
	LuaArgBundle::CountPushed++;
	if (LuaArgBundle::CountPushed > LuaArgBundle::PeakCount + LuaArgBundle::CountDeleted) LuaArgBundle::PeakCount = LuaArgBundle::CountPushed - LuaArgBundle::CountDeleted;
#endif
}

#ifdef _BENCHMARK_OBJ_COUNTERS_
LuaArgBundle::~LuaArgBundle() 
{
	LuaArgBundle::CountDeleted++;
}
#endif
	
LuaArgBundle::LuaArgBundle(lua_State* pL, int height, int maxTableDepth)
{
	int heightDelta = 0;
	
	size_t len;
	const char* str;
	
	while (heightDelta < height)
	{

		std::unique_ptr<LuaArgUnpackStep> stepToAddValue = nullptr;

		int luaType = lua_type(pL, -1);

		switch (luaType)
		{
		case LUA_TNUMBER:
			stepToAddValue = std::make_unique<LuaArgNum>(lua_tonumber(pL,-1)); 
			break;
		case LUA_TBOOLEAN:
			stepToAddValue = std::make_unique<LuaArgBool>(lua_toboolean(pL,-1)); 
			break;
		case LUA_TSTRING:
			str = lua_tolstring(pL, -1, &len);
			stepToAddValue = std::make_unique<LuaArgStr>(str,len); 
			break;

		case LUA_TTABLE:
			BundleTable(pL, maxTableDepth); // Pop table and append instructions for building a copy to mArgs
			heightDelta++;
			continue;
		default: 
			stepToAddValue = std::make_unique<LuaArgNil>(); 
			break;
		}

		heightDelta++;
		lua_pop(pL,1);

		mArgs.push_back(std::move(stepToAddValue));
	}

#ifdef _BENCHMARK_OBJ_COUNTERS_
	LuaArgBundle::CountPushed++;
	if (LuaArgBundle::CountPushed > LuaArgBundle::PeakCount + LuaArgBundle::CountDeleted) LuaArgBundle::PeakCount = LuaArgBundle::CountPushed - LuaArgBundle::CountDeleted;
#endif
}

int LuaArgBundle::Unpack(lua_State* pL)
{
	int height = 0;

	for (auto it = mArgs.crbegin(); it != mArgs.crend(); ++it)
	{
		height += it->get()->Unpack(pL);
	}
	return height;
}

//------------------------------
// Diagnostic statics
//------------------------------
#ifdef _BENCHMARK_OBJ_COUNTERS_
		std::atomic<int> LuaArgBundle::CountPushed = 0;
		std::atomic<int> LuaArgBundle::CountDeleted = 0;
		std::atomic<int> LuaArgBundle::PeakCount = 0;
#endif
