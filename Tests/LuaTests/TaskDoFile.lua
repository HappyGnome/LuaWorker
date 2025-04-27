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

MyString = "Hello From File Task"

w = LuaWorker.Create()
w:Start()

Step1 = function()
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()
	T = w:DoFile(RootDir .. "\\TaskDoFile_FileTask.lua")

	local res ,t = T:Await(500)

	RaiseFirstWorkerError(w)

	return (T:Status() == LuaWorker.TaskStatus.Complete) and (res == "File Run") and DeepMatch(t,{1,{2,{3}}})
end 

Step3 = function()
	T2 = w:DoString("return MirrorString([[".. MyString .. "]])")

	RaiseFirstWorkerError(w)
	return T2:Finalized() == false
end 

Step4 = function()

	local res = T2:Await(500)

	RaiseFirstWorkerError(w)
	return (T:Status() == LuaWorker.TaskStatus.Complete) and (T2:Status() == LuaWorker.TaskStatus.Complete) and (res == MyString)
end

Step5 = function()
	T3 = w:DoFile({maxTableDepth = 1},RootDir .. "\\TaskDoFile_FileTask.lua")

	local res ,t = T3:Await(500)

	RaiseFirstWorkerError(w)

	return (T3:Status() == LuaWorker.TaskStatus.Complete) and (res == "File Run") and DeepMatch(t,{1})
end 

Step6 = function()
	T4 = w:DoFile({maxTableDepth = 2},RootDir .. "\\TaskDoFile_FileTask.lua")

	local res ,t = T4:Await(500)

	RaiseFirstWorkerError(w)

	if (T4:Status() == LuaWorker.TaskStatus.Complete) and (res == "File Run") and DeepMatch(t,{1,{2}}) then
		return true
	else
		error(obj2str(t))
	end
end 

