# LuaWorker

A simple/lightweight Lua threading module. 

N.B. This project is in an early state, and may need polish and testing in some places.

## Lua Usage Example
```
package.cpath = package.cpath..";"..LuaWorker.dll;"

require('LuaWorker')

local w = LuaWorker.Create() -- Create worker

w:Start()	-- Start worker thread with separate lua state

local status, task = w:DoString(
		[[
			local i = 0

			print("In worker thread")

			while i < 10000000 do
				i = (i + 10000002)%10000001 -- i+1 (slowly)
			end

			print("End of worker thread")

			return 'Done'
		]])	-- Execute string in parallel lua thread

local i = 0
while i < 5000000 do
	i = (i + 10000002)%10000001 -- i+1 (slowly)
end

print("Still in calling thread") -- Demo that we're still executing here

print(task:Await()) -- Wait for task to complete and print result

-- Readout any log entries
while (true) do
	local s = LuaWorker.PopLogLine()
	if s == nil then break end
	print(s)		
end
```

Console output:

```
In worker thread
Still in calling thread
End of worker thread
Done
2023-10-01 19:48:42Z | Worker 0 | Info | Thread start requested.
2023-10-01 19:48:42Z | Worker 0 | Info | Thread starting.
```