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

MyString = "Hello From Task"

GetNestedTable = "return {1,{2,{3}}}, 99"

w = LuaWorker.Create()
w:Start()

Step1 = function()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()
	T = w:DoString("return [[" .. MyString .. "]]")

	RaiseFirstWorkerError(w)
	return true
end 

Step3 = function()

	local res = T:Await(500)

	RaiseFirstWorkerError(w)
	if not ( (T:Status() == LuaWorker.TaskStatus.Complete) and (res == MyString)) then
		error(res)
	else
		return true
	end
end

Step3 = function()
	T = w:DoString(GetNestedTable)

	local t1,t2 = T:Await(500)

	RaiseFirstWorkerError(w)

	if t2 ~= 99 then
		error("t2: " .. t2)
	elseif  not DeepMatch(t1,{1,{2,{3}}}) then
		error (obj2str(t1))
	end

	return true
end 

Step4 = function()
	T = w:DoString({maxTableDepth = 0},GetNestedTable)

	local t1,t2 = T:Await(500)

	RaiseFirstWorkerError(w)

	if t2 ~= 99 then
		error("t2: " .. t2)
	elseif  not DeepMatch(t1,nil) then
		error (obj2str(t1))
	end

	return true
end 

Step5 = function()
	T = w:DoString({maxTableDepth = 1},GetNestedTable)

	local t1,t2 = T:Await(500)

	RaiseFirstWorkerError(w)

	if t2 ~= 99 then
		error("t2: " .. t2)
	elseif  not DeepMatch(t1,{1}) then
		error (obj2str(t1))
	end

	return true
end 

Step6 = function()
	T = w:DoString({maxTableDepth = 2},GetNestedTable)

	local t1,t2 = T:Await(500)

	RaiseFirstWorkerError(w)

	if t2 ~= 99 then
		error("t2: " .. t2)
	elseif  not DeepMatch(t1,{1, {2}}) then
		error (obj2str(t1))
	end

	return true
end 

