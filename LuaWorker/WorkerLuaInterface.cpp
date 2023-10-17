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

#include "WorkerLuaInterface.h"
#include "TaskDoFile.h"
#include "TaskDoString.h"
#include "TaskDoSleep.h"

using namespace LuaWorker;
using namespace AutoKeyDeck;
//-------------------------------
// Static private objects
//-------------------------------

AutoKeyMap<lua_Integer, Worker> WorkerLuaInterface::sWorkers(0);

lua_Integer WorkerLuaInterface::sNextWorkerId = 0;

//-------------------------------
// Static Lua helper methods
//-------------------------------
std::shared_ptr<Worker> WorkerLuaInterface::l_PopWorker(lua_State* pL)
{
	int k;
	return l_PopWorker(pL, k);
}

std::shared_ptr<Worker> WorkerLuaInterface::l_PopWorker(lua_State* pL, int& outKey)
{
	if (!lua_isnumber(pL, lua_upvalueindex(1))) return nullptr;
	int key = (int)lua_tointeger(pL, lua_upvalueindex(1));

	try
	{
		outKey = key;

		return sWorkers.at(key);
	}
	catch (std::out_of_range)
	{
		return nullptr;
	}
}

//-------------------------------
// Static Lua-callable methods 
// (Library level)
//-------------------------------

int WorkerLuaInterface::l_Worker_Create(lua_State* pL)
{
	lua_Integer logSize = 100;
	if (lua_isnumber(pL, -1))
	{
		logSize = lua_tointeger(pL, -1);
	}

	LogSection log(std::make_shared<LogStack>(logSize), "Worker " + std::to_string(sNextWorkerId++));

	std::shared_ptr<Worker> newWorker(new Worker(std::move(log)));

	lua_Integer key = sWorkers.push(newWorker);

	lua_createtable(pL, 0, 5);
	lua_newuserdata(pL, 1);
	lua_createtable(pL, 0, 1);
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_Delete, 1);
	lua_setfield(pL, -2, "__gc");
	lua_setmetatable(pL, -2);
	lua_setfield(pL, -2, "destructor");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_DoString, 1);
	lua_setfield(pL, -2, "DoString");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_DoFile, 1);
	lua_setfield(pL, -2, "DoFile");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_DoSleep, 1);
	lua_setfield(pL, -2, "DoSleep");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_Start, 1);
	lua_setfield(pL, -2, "Start");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_Stop, 1);
	lua_setfield(pL, -2, "Stop");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_Status, 1);
	lua_setfield(pL, -2, "Status");
	lua_pushinteger(pL, key);
	lua_pushcclosure(pL, l_Worker_PopLogLine, 1);
	lua_setfield(pL, -2, "PopLogLine");

	return 1;
}

//-------------------------------
// Static Lua-callable methods 
// (Worker Object)
//-------------------------------
int WorkerLuaInterface::l_Worker_Delete(lua_State* pL) 
{
	int key{};
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL, key);

	if (pWorker != nullptr)
	{
		pWorker->Stop();
		sWorkers.pop(key);
	}
	return 0;
}

int WorkerLuaInterface::l_Worker_DoString(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker != nullptr)
	{
		if (lua_isstring(pL, -1))
		{
			std::string str = lua_tostring(pL, -1);

			std::shared_ptr<Task> newItem(new TaskDoString(str));
			pWorker->AddTask(newItem);

			return TaskLuaInterface::l_PushTask(pL, newItem);
		}
	}

	return 0;
}

int WorkerLuaInterface::l_Worker_DoFile(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker != nullptr)
	{
		if (lua_isstring(pL, -1))
		{
			std::string str = lua_tostring(pL, -1);

			std::shared_ptr<Task> newItem(new TaskDoFile(str));
			pWorker->AddTask(newItem);
			
			return TaskLuaInterface::l_PushTask(pL, newItem);
		}
	}

	return 0;
}

int WorkerLuaInterface::l_Worker_DoSleep(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker != nullptr)
	{
		if (lua_isnumber(pL, -1))
		{
			unsigned int millis = std::max(0,(int)lua_tointeger(pL, -1));

			std::shared_ptr<Task> newItem(new TaskDoSleep(millis));
			pWorker->AddTask(newItem);

			return TaskLuaInterface::l_PushTask(pL, newItem);
		}
	}

	return 0;
}

int WorkerLuaInterface::l_Worker_Status(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	return l_PushStatus(pL, pWorker);
}

int WorkerLuaInterface::l_PushStatus(lua_State* pL, std::shared_ptr<Worker> pWorker)
{
	if (pWorker == nullptr) return 0;

	WorkerStatus status = pWorker-> GetStatus();

	int statusInt;

	switch (status)
	{
	case WorkerStatus::Cancelled:	statusInt = WorkerStatus_Cancelled;		break;
	case WorkerStatus::Processing:	statusInt = WorkerStatus_Processing;	break;
	case WorkerStatus::Starting:	statusInt = WorkerStatus_Starting;		break;
	case WorkerStatus::Error:		statusInt = WorkerStatus_Error;			break;
	default:						statusInt = WorkerStatus_NotStarted;	break;
	}

	lua_pushinteger(pL, statusInt);

	return 1;
}

int WorkerLuaInterface::l_Worker_Start(lua_State* pL) 
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker != nullptr) pWorker->Start();

	return l_PushStatus(pL, pWorker);
}

int WorkerLuaInterface::l_Worker_Stop(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker != nullptr) pWorker->Stop();

	return l_PushStatus(pL, pWorker);
}

int WorkerLuaInterface::l_Worker_PopLogLine(lua_State* pL)
{
	std::shared_ptr<Worker> pWorker = l_PopWorker(pL);

	if (pWorker == nullptr) return 0;

	std::string msg;
	LogLevel level;

	if (pWorker->GetLogOutput()->PopLine(msg, level))
	{
		lua_pushstring(pL, msg.c_str());

		int luaLevel;
		switch (level)
		{
		case LogLevel::Info: luaLevel = LogLevel_Info; break;
		case LogLevel::Error: luaLevel = LogLevel_Error; break;
		case LogLevel::Warn: luaLevel = LogLevel_Warn; break;
		default: luaLevel = LogLevel_Info; break;
		}
		lua_pushinteger(pL, luaLevel);

		return 2;
	}

	return 0;
}
