# LuaWorker

## Introduction
LuaWorker aims to be a clean and lightweight Lua threading module. The original motivation for this project was the need to write a mod for a game running Lua 5.1.5 which could trigger asynchronous tasks (e.g. http requests) from the main game loop.

## Lua versions
LuaWorker is initially targeted at [Lua 5.1.5](https://www.lua.org/versions.html), but it should be possible to adapt it for later versions. Required lua source to build LuaWorker for Lua 5.1.5 and to test it on Windows 10+ are included.

## Build tools
The included solution file for Microsoft Visual Studio can be used to build LuaWorker and test/example projects. The following software/language versions have been used for this project:
* Visual Studio 2022
* Build tools ('Platform Toolset') v143 
* C++ 17 standard

## Installation
* Build the LuaWorker project
* Copy the output `LuaWorker.dll` to a directory your lua instance can access.
 
## Usage quickstart
From your lua scripts, add Luaworker to your environment:
* Add the path to `LuaWorker.dll` to `package.cpath`. E.g. `package.cpath = package.cpath..";LuaWorker.dll;"`
* Call `require('LuaWorker')`

Use LuaWorker to execute lua on a new thread:
* Create a LuaWorker with `LuaWorker.Create()`. E.g. `local worker = LuaWorker.Create()`
* Start the worker thread with `worker:Start()`
* Send a task to the worker using `worker:DoString`, `worker:DoFile` or `worker:DoSleep`
* Await task completion (for a limited time) on the main thread with `task:Await`
* Stop the thread, and cancel queued tasks with `worker:Stop()`

See the [full example](#lua-usage-example) below, and the [reference document](Docs/LuaReference.md) for full details.

## Lua usage example
```
package.cpath = package.cpath..";".."LuaWorker.dll;"

require('LuaWorker')

-- Create worker
local worker = LuaWorker.Create()			

-- Start worker thread with separate lua state
worker:Start()								

-- Execute string in parallel lua thread
local task = worker:DoString(
		[[
			print("Work started")

			local i = 0

			while i < 10000000 do
				i = (i + 10000002)%10000001 -- i+1 (slowly)
			end

			print("Work done")

			return 'Worker task returned'
		]])									

-- Do some other work in calling thread
local i = 0
while i < 5000000 do
	i = (i + 10000002)%10000001				 
end

-- Demo that we're still executing here
print("Still in calling thread")			

-- Wait for up to 1 second for parallel task to complete
local res = task:Await(1000)				

-- Prints 'Worker task done'
if task:Finalized() then
	print(res)								
else
	print("Task not finished in time!")
end

worker:Stop()

print("\nWorker logs: ")

-- Readout any log entries
while (true) do								
	local s = LuaWorker.PopLogLine()
	if s == nil then break end
	print(s)		
end
```

Console output:

```
Work started
Still in calling thread
Work done
Worker task returned

Worker logs:
2023-10-07 21:09:21Z | Worker 0 | Info | Thread start requested.
2023-10-07 21:09:21Z | Worker 0 | Info | Thread starting.
2023-10-07 21:09:21Z | Worker 0 | Info | Lua opened on worker.
2023-10-07 21:09:21Z | Worker 0 | Info | Thread stop requested.
2023-10-07 21:09:21Z | Worker 0 | Info | Thread stopping.
```

## License and copyright
This project is released under an Apache 2.0 license, as set out in [LICENSE.txt](LICENSE.txt). 

Individual contributor copyright:
* Copyright 2023 HappyGnome
