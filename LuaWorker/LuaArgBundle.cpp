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

int LuaArgUnpackStep::Unpack(lua_State* pL) const
{
	switch (mType)
	{
	case LuaArgStepType::Number:
		lua_pushnumber(pL, std::get<lua_Number>(mData));
		return 1;
	case LuaArgStepType::Bool:
		lua_pushboolean(pL, std::get<int>(mData));
		return 1;
	case LuaArgStepType::Nil:
		lua_pushnil(pL);
		return 1;
	case LuaArgStepType::String:
		{
			const std::string& s = std::get<std::string>(mData);
			lua_pushlstring(pL, s.data(), s.size());
			return 1;
		}
	case LuaArgStepType::TableSet:
		lua_rawset(pL, -3);
		return -2;
	case LuaArgStepType::TableStart:
		{
			const LTableSpec& t = std::get<LTableSpec>(mData);
			lua_createtable(pL, t.NArr, t.NRec);
			return 1;
		}
	default:
		return 0;
	}
}

LuaArgUnpackStep LuaArgUnpackStep::PushNumber(lua_Number value)
{
	LuaArgUnpackStep result;

	result.mType = LuaArgStepType::Number;
	result.mData = value;

	return result;
}

LuaArgUnpackStep LuaArgUnpackStep::PushNil()
{
	LuaArgUnpackStep result;

	result.mType = LuaArgStepType::Nil;

	return result;
}

LuaArgUnpackStep LuaArgUnpackStep::PushBool(int value)
{
	LuaArgUnpackStep result;

	result.mType = LuaArgStepType::Bool;
	result.mData = value;

	return result;
}

LuaArgUnpackStep LuaArgUnpackStep::PushString(const char* value, size_t len)
{
	LuaArgUnpackStep result;
	
	result.mType = LuaArgStepType::String;
	result.mData = std::string(value, len);

	return result;
}

LuaArgUnpackStep LuaArgUnpackStep::SetTable()
{
	LuaArgUnpackStep result;

	result.mType = LuaArgStepType::TableSet;

	return result;
}

LuaArgUnpackStep LuaArgUnpackStep::StartTable(int nArr, int nRec)
{
	LuaArgUnpackStep result;

	result.mType = LuaArgStepType::TableStart;
	result.mData = LTableSpec{ nArr, nRec };

	return result;
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
		mArgs.push_back(LuaArgUnpackStep::PushNil()); 
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
		if (!skipKey)
		{
			switch (lua_type(pL, -1))
			{
			case LUA_TNUMBER:
				mArgs.push_back(LuaArgUnpackStep::PushNumber(lua_tonumber(pL, -1)));
				break;
			case LUA_TBOOLEAN:
				mArgs.push_back(LuaArgUnpackStep::PushBool(lua_toboolean(pL, -1)));
				break;
			case LUA_TSTRING:
				str = lua_tolstring(pL, -1, &len);
				mArgs.push_back(LuaArgUnpackStep::PushString(str, len));
				break;
			default:
				assert(false); // skipKey should have been set, and instructions to create the corresponding value not added if this key is invalid. 
			}
		}

		skipKey = false;

		if (lua_next(pL, -2) == 0)
		{
			TableSizeCounter tsc = parentSizes[tableDepth];
			mArgs.push_back(LuaArgUnpackStep::StartTable(tsc.nArr, std::max(0,tsc.nRec - tsc.nArr)));

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

		mArgs.push_back(LuaArgUnpackStep::SetTable());
		parentSizes[tableDepth].nRec++;

		switch (lua_type(pL, -1)) // value type
		{
		case LUA_TNUMBER:
			mArgs.push_back(LuaArgUnpackStep::PushNumber(lua_tonumber(pL, -1)));
			break;
		case LUA_TBOOLEAN:
			mArgs.push_back(LuaArgUnpackStep::PushBool(lua_toboolean(pL, -1)));
			break;
		case LUA_TSTRING:
			str = lua_tolstring(pL, -1, &len);
			mArgs.push_back(LuaArgUnpackStep::PushString(str,len));
			break;

		case LUA_TTABLE:
			if (tableDepth >= maxTableDepth)
			{
				mArgs.push_back(LuaArgUnpackStep::PushNil());
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
			mArgs.push_back(LuaArgUnpackStep::PushNil());
			break;
		}

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

		int luaType = lua_type(pL, -1);

		switch (luaType)
		{
		case LUA_TNUMBER:
			mArgs.push_back(LuaArgUnpackStep::PushNumber(lua_tonumber(pL, -1)));
			break;
		case LUA_TBOOLEAN:
			mArgs.push_back(LuaArgUnpackStep::PushBool(lua_toboolean(pL, -1)));
			break;
		case LUA_TSTRING:
			str = lua_tolstring(pL, -1, &len);
			mArgs.push_back(LuaArgUnpackStep::PushString(str,len));
			break;

		case LUA_TTABLE:
			BundleTable(pL, maxTableDepth); // Pop table and append instructions for building a copy to mArgs
			heightDelta++;
			continue;
		default: 
			mArgs.push_back(LuaArgUnpackStep::PushNil());
			break;
		}

		heightDelta++;
		lua_pop(pL,1);

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
		height += it->Unpack(pL);
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
