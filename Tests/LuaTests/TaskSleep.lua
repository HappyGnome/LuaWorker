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

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoSleep(500) -- Prevent main wait fromn starting during step 2

	RaiseFirstWorkerError()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()
	T = w:DoSleep(2000)

	RaiseFirstWorkerError()
	return (T:Status() == LuaWorker.TaskStatus.NotStarted) and (T:Finalized() == false)
end 

-- Delay 1000

Step3 = function()
	return T:Status() == LuaWorker.TaskStatus.Running
end 

Step4 = function()
	T:Await(2000)

	RaiseFirstWorkerError()
	return (T:Status() == LuaWorker.TaskStatus.Complete) and (T:Finalized() == true)
end 

Step5 = function()
	w:Stop()

	RaiseFirstWorkerError()
	return w:Status() == LuaWorker.WorkerStatus.Cancelled
end 
