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
	T = w:DoString("InLuaWorker.Sleep(1000) return 'ExecAfterSleep'")

	if T:Await(1300) ~= 'ExecAfterSleep' then return false end
	return T:Status() == LuaWorker.TaskStatus.Complete
end 

Step3 = function()

	T = w:DoString("InLuaWorker.Sleep(5000)")

	T:Await(2000)
	if T:Status() ~= LuaWorker.TaskStatus.Running then return false end

	-- Should not block the remaining 4 seconds
	w:Stop()

	RaiseFirstWorkerError(w)
	return w:Status() == LuaWorker.WorkerStatus.Cancelled
end