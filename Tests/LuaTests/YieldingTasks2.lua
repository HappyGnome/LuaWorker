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

YieldingFunc = function(ch, delay)
	for i = 1,5 do
		gStr = gStr .. ch
		InLuaWorker.YieldFor(delay,gStr)
	end
	return gStr
end]]

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoString(initStr):Await(500)

	RaiseFirstWorkerError(w)
	
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()

	T1 = w:DoCoroutine("YieldingFunc","a","1000")

	RaiseFirstWorkerError(w)
	return true
end 

Step3 = function()
	T2 = w:DoCoroutine("YieldingFunc","b","150")

	RaiseFirstWorkerError(w)
	return true
end 

--After 5s
Step4 = function()
	T = w:DoString("return gStr")
	res = T:Await(200) 

	RaiseFirstWorkerError(w)
	if res == "abbbbbaaaa" then 
		return true
	else
		error(res)
	end
end 