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

w = LuaWorker.Create()
w:Start()

Step1 = function()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()
	T = w:DoString("NOT LUA")	-- Should error
	T2 = w:DoSleep(1000)		-- Should execute after the error

	--RaiseFirstWorkerError() 
	return true
end 

Step3 = function()
	
T:Await(500)

	-- RaiseFirstWorkerError()
	return T:Status() == LuaWorker.TaskStatus.Error
end 

--Should raise error
Step4 = function()

	RaiseFirstWorkerError()
	return true
end 

-- Should take ~1s
Step5 = function()

	T2:Await(1100)

	RaiseFirstWorkerError()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 