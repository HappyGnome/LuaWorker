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

#include "TaskLuaInterface.h"

using namespace LuaWorker;

AutoKeyMap<int, Task> TaskLuaInterface::mTasks(0);

//-------------------------------
// Static Lua helper methods
//-------------------------------
std::shared_ptr<Task> TaskLuaInterface::l_PopTask(lua_State* pL)
{
	int k;
	return l_PopTask(pL, k);
}

std::shared_ptr<Task> TaskLuaInterface::l_PopTask(lua_State* pL, int &outKey)
{
	if (!lua_isnumber(pL, lua_upvalueindex(1))) return nullptr;
	int key = (int)lua_tointeger(pL, lua_upvalueindex(1));

	try 
	{
		outKey = key;

		return mTasks.at(key);
	}
	catch (std::out_of_range) 
	{
		return nullptr;
	}
}

//-------------------------------
// Public Static Methods
//-------------------------------

int TaskLuaInterface::l_PushTask(lua_State* pL, std::shared_ptr<Task> pTask)
{
	int key = mTasks.push(pTask);

	lua_createtable(pL, 0, 5);
		lua_newuserdata(pL, 1);
			lua_createtable(pL, 0, 1);
				lua_pushinteger(pL, key);
				lua_pushcclosure(pL, l_Task_Delete, 1);
			lua_setfield(pL, -2, "__gc");
		lua_setmetatable(pL, -2);
	lua_setfield(pL, -2, "destructor");
		lua_pushinteger(pL, key);
		lua_pushcclosure(pL, l_Task_Await, 1);
	lua_setfield(pL, -2, "Await");
		lua_pushinteger(pL, key);
		lua_pushcclosure(pL, l_Task_Status, 1);
	lua_setfield(pL, -2, "Status");

	return 1;
}


//-------------------------------
// Private methods
//-------------------------------


//-------------------------------
// Public Static Lua-callable methods
//-------------------------------
int TaskLuaInterface::l_Task_Delete(lua_State* pL)
{
	int key;
	std::shared_ptr<Task> p = l_PopTask(pL, key);

	if (p != nullptr)
	{
		mTasks.pop(key);
	}

	return 0;
}

int TaskLuaInterface::l_Task_Await(lua_State* pL)
{
	std::shared_ptr<Task> p = l_PopTask(pL);

	if (p != nullptr)
	{
		p->WaitForResult();

		if (p->GetStatus() == TaskStatus::Complete)
		{
			lua_pushstring(pL, p->GetResult().c_str());
			return 1;
		}

	}

	return 0;
}

int TaskLuaInterface::l_Task_Status(lua_State* pL)
{
	std::shared_ptr<Task> p = l_PopTask(pL);

	if (p != nullptr)
	{
		TaskStatus status = p->GetStatus();

		int statusInt;

		switch (status)
		{
		case TaskStatus::Cancelled: statusInt = TaskStatus_Cancelled; break;
		case TaskStatus::Complete: statusInt = TaskStatus_Complete; break;
		case TaskStatus::Running: statusInt = TaskStatus_Running; break;
		case TaskStatus::Error: statusInt = TaskStatus_Error; break;
		default: statusInt = TaskStatus_NotStarted; break;
		}

		lua_pushinteger(pL, statusInt);

		return 1;
	}

	return 0;
}

