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

package.cpath = package.cpath..";"..RootDir.."\\?.dll;"

require('LuaWorker')

-----------------------------------------
RaiseFirstWorkerError = function()
	while (true) do
		local s, l = LuaWorker.PopLogLine()
		if s == nil then break end
		if l == LuaWorker.LogLevel.Error then
			error(s)
		end
	end
end
----------------------------------------

MyString = "Hello From Task"

w = LuaWorker.Create()
w:Start()

Step1 = function()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()
	T = w:DoString("return [[" .. MyString .. "]]")

	RaiseFirstWorkerError()
	return true
end 

Step3 = function()

	while not T:Finalized() do
		 T:Await(50)
	end

	local res = T:Await(50)

	RaiseFirstWorkerError()
	if not ( (T:Status() == LuaWorker.TaskStatus.Complete) and (res == MyString)) then
		error(res)
	else
		return true
	end
end