# LuaWorker

A simple/lightweight Lua threading module. 

## Lua Usage Example
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
			local i = 0

			while i < 10000000 do
				i = (i + 10000002)%10000001 -- i+1 (slowly)
			end

			return 'Worker task done'
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

-- Readout any log entries
while (true) do								
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
