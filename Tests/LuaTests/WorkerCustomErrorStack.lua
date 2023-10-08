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

w = LuaWorker.Create(50)
w:Start()

logErrorLast = 
	[[
		InLuaWorker.LogError("Error in log!")

		for i = 1,49 do
			InLuaWorker.LogInfo("Log entry " .. i)
		end
	]]

Step1 = function()
	RaiseFirstWorkerError(w)
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

-- Should raise error
Step2 = function()
	T = w:DoString(logErrorLast)

	local res = T:Await(500)

	RaiseFirstWorkerError(w)

	return true
end 

-- Should not raise error
Step3 = function()
	w:DoString(logErrorLast)
	T = w:DoString([[InLuaWorker.LogInfo("Push error off stack")]])

	local res = T:Await(500)

	RaiseFirstWorkerError(w)

	return true
end 