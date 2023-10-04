--[[*****************************************************************************
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
]]--*****************************************************************************

package.cpath = package.cpath..";"..BinDir.."\\?.dll;"

require('LuaWorker')

--os.execute('sleep 10')

foo = function()
	local w = LuaWorker.Create()
	w:Start()
	local status, task = w:DoString(
		[[
			local i = 0

			print("In worker thread")

			while i < 10000000 do
				i = (i + 10000002)%10000001 -- +1 slowly
			end

			print("End of worker thread")

			return 'Done'
		]])

	local i = 0

	while i < 5000000 do
		i = (i + 10000002)%10000001 -- +1 slowly
	end

	print("Still in calling thread")

	task:Await(10000)

	local status, task = w:DoFile([[file_to_run.lua]])

	task:Await(1000)

	w:Stop()
end

foo();

while (true) do
	local s = LuaWorker.PopLogLine()
	if s == nil then break end
	print(s)		
end