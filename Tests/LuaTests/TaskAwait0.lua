--[[*****************************************************************************
* 
*  Copyright 2025 HappyGnome
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

	RaiseFirstWorkerError(w)
	if w:Status() ~= LuaWorker.WorkerStatus.Processing then
		error (w:Status())
	else
		return true
	end
end 

Step2 = function()
	T1 = w:DoString("InLuaWorker.Sleep(10) return 'New Result'")
	T2 = w:DoString("InLuaWorker.Sleep(10) return 'New Result'")
	T3 = w:DoString("InLuaWorker.Sleep(10) return 'New Result'")



	if T1:Await() then error("Nil Await has result") end
	if T2:Await(0) then error("0 Await has result") end
	if T3:Await(-100000) then error("Negative Await has result") end

	return true  
end

	-- After delay
Step3 = function()
	if not T1:Await() then error("Nil Await has no result") end
	if not T2:Await(0) then error("0 Await has no result") end
	if not T3:Await(-100000) then error("Negative Await has no result") end

	return true
end 


