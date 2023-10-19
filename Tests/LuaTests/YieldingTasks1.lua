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

initstr = [[gStr = ""

YieldingFunc = function(ch)
	for i = 1,5 do
		gStr = gStr .. ch
		InLuaWorker.YieldFor(true,1000)
	end
	return true
end]]

Step1 = function()
	w = LuaWorker.Create()

	w:DoString(initStr)

	RaiseFirstWorkerError(w)
	
	return true
end 

Step2 = function()

	w:DoString("return [[YieldingFunc('a')]]")

	RaiseFirstWorkerError(w)
	return true
end 

Step3 = function()
	w:DoString("return [[YieldingFunc('b')]]")

	RaiseFirstWorkerError(w)
	return true
end 

Step4 = function()
	T = w:DoString("return gStr") 
	res = T:Await(5000) 
	error(res) -- TODO
	return res== "ab"
end 

--After 1s
Step5 = function()
	T = w:DoString("return gStr") 
	return T:Await(500) == "abab"
end 

--After 1s more
Step6 = function()
	T = w:DoString("return gStr") 
	return T:Await(500) == "ababab"
end 

--After 3s more
Step7 = function()
	T = w:DoString("return gStr") 
	return T:Await(500) == "ababababab"
end 