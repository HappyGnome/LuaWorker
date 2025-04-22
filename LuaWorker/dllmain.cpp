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

// dllmain.cpp : Defines the entry point for the DLL application.

#include "WorkerLuaInterface.h"
#include "TaskLuaInterface.h"

extern "C" {
    #include "lua.h"
    //#include "lauxlib.h"
    //#include "lualib.h"
}

using namespace LuaWorker;

extern "C"  int luaopen_LuaWorker(lua_State* pL) {
    static const luaL_Reg Worker_Index[] = {
          {"Create", WorkerLuaInterface::l_Worker_Create},
          {"Version", WorkerLuaInterface::l_LuaWorker_Version},

          {nullptr, nullptr}  /* end */
    };
    luaL_register(pL, "LuaWorker", Worker_Index);

    // Add Enums

    //Task Status
        lua_createtable(pL, 0, 4);
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_NotStarted);
        lua_setfield(pL, -2, "NotStarted");
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_Cancelled);
        lua_setfield(pL, -2, "Cancelled");
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_Complete);
        lua_setfield(pL, -2, "Complete");
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_Running);
        lua_setfield(pL, -2, "Running");
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_Error);
        lua_setfield(pL, -2, "Error");
            lua_pushnumber(pL, TaskLuaInterface::TaskStatus_Suspended);
        lua_setfield(pL, -2, "Suspended");
    lua_setfield(pL, -2, "TaskStatus");

    //Worker Status
        lua_createtable(pL, 0, 4);
            lua_pushnumber(pL, WorkerLuaInterface::WorkerStatus_NotStarted);
        lua_setfield(pL, -2, "NotStarted");
            lua_pushnumber(pL, WorkerLuaInterface::WorkerStatus_Starting);
        lua_setfield(pL, -2, "Starting");
            lua_pushnumber(pL, WorkerLuaInterface::WorkerStatus_Processing);
        lua_setfield(pL, -2, "Processing");
            lua_pushnumber(pL, WorkerLuaInterface::WorkerStatus_Cancelled);
        lua_setfield(pL, -2, "Cancelled");
            lua_pushnumber(pL, WorkerLuaInterface::WorkerStatus_Error);
        lua_setfield(pL, -2, "Error");
    lua_setfield(pL, -2, "WorkerStatus");

    //Log Level
    lua_createtable(pL, 0, 4);
        lua_pushnumber(pL, WorkerLuaInterface::LogLevel_Info);
    lua_setfield(pL, -2, "Info");
        lua_pushnumber(pL, WorkerLuaInterface::LogLevel_Warn);
        lua_setfield(pL, -2, "Warn");
    lua_pushnumber(pL, WorkerLuaInterface::LogLevel_Error);
        lua_setfield(pL, -2, "Error");
    lua_setfield(pL, -2, "LogLevel");

    return 1;
}

