--[[**************************************************************************\
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
]]--**************************************************************************/

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
	local s = worker:PopLogLine()
	if s == nil then break end
	print(s)		
end