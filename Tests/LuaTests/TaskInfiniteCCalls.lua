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
	T = w:DoFile(RootDir .. "\\TaskInfiniteCCalls_FileTask.lua")

	local res = T:Await(2000)

	RaiseFirstWorkerError(w)

	return T:Status() == LuaWorker.TaskStatus.Running
end 

-- Should throw error
Step3 = function()
	w:Stop()

	T:Await(2000)

	RaiseFirstWorkerError(w)
	return true
end 

Step4 = function()
	return (T:Status() == LuaWorker.TaskStatus.Error)
end