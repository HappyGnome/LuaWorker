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

initStr = [[gStr = ""

YieldingFunc = function(ch)
	for i = 1,5 do
		gStr = gStr .. ch
		InLuaWorker.YieldFor(1000,true)
	end
	return true
end]]

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoString(initStr):Await(500)

	RaiseFirstWorkerError(w)
	
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()

	T = w:DoString("return YieldingFunc('a')")
	
	res = T:Await(1000)
	return T:Status()  == LuaWorker.TaskStatus.Error and res == nil
end 

Step3 = function()

	RaiseFirstWorkerError(w) -- Should be an error "Cannot yield here"
	return true
end 