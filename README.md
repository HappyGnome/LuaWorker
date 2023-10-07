# LuaWorker

A simple/lightweight Lua threading module. 

## Lua Usage Example
```
package.cpath = package.cpath..";".."LuaWorker.dll;"

require('LuaWorker')

local worker = LuaWorker.Create()			-- Create worker

worker:Start()								-- Start worker thread with separate lua state

local task = worker:DoString(
		[[
			local i = 0

			while i < 10000000 do
				i = (i + 10000002)%10000001 -- i+1 (slowly)
			end

			return 'Worker task done'
		]])									-- Execute string in parallel lua thread

local i = 0
while i < 5000000 do
	i = (i + 10000002)%10000001				 -- i+1 (slowly)
end

print("Still in calling thread")			-- Demo that we're still executing here

local res = task:Await(1000)				-- Wait for up to 1 second for task to complete and print result

if task:Finalized() then
	print(res)								-- Prints 'Worker task done'
else
	print("Task not finished in time!")
end

worker:Stop()
											
while (true) do								-- Readout any log entries
	local s = LuaWorker.PopLogLine()
	if s == nil then break end
	print(s)		
end
```

Console output:

```
Still in calling thread
Worker task done
2023-10-07 20:57:30Z | Worker 0 | Info | Thread start requested.
2023-10-07 20:57:30Z | Worker 0 | Info | Thread starting.
2023-10-07 20:57:30Z | Worker 0 | Info | Lua opened on worker.
2023-10-07 20:57:30Z | Worker 0 | Info | Thread stop requested.
2023-10-07 20:57:30Z | Worker 0 | Info | Thread stopping.
```