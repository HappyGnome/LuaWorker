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