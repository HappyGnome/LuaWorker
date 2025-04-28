
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

initStr = [[


Echo = function(t)

	local s = ""
	for k,v in pairs(t) do
		if v~= nil then
			if type(v) == 'boolean' then
				if v then
					v = "true"
				else
					v= "false"
				end
			end
			s = s .. k .. v
		end
	end	

	return s
end

Reflect = function(...)

	arg["n"] = nil
	return arg, "AnotherResult"
end

ArgC = function(...)

	return #arg
end

]]

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoString(initStr)

	RaiseFirstWorkerError(w)
	
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

-- Nested tables, various
Step2 = function()

	local toSend = {"1",99, {{"Hello"}}, nil, {"7",{true}}, {false}}
	
	T1 = w:DoCoroutine("Reflect",unpack(toSend))

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)

	local ok, str = DeepMatch(toSend,res)
	if  ok then
		return true
	else
		error("Mismatch at " .. str .. " :: " .. obj2str(res))
	end
end 

-- Nested tables non-tree
Step3 = function()

	local toSend = {}
	toSend[1] = toSend
	toSend[2] = "2"
	
	T1 = w:DoCoroutine({maxTableDepth = 10}, "Reflect",unpack(toSend))

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)

	local ok , str = DeepMatch(toSend,res,10)
	if  ok then
		return true
	else
		error("Mismatch at " .. str .. " :: " .. obj2str(res))
	end
end 

-- Nested tables non-tree, diferent depth
Step4 = function()

	local toSend = {toSend}
	toSend[1] = toSend
	
	T1 = w:DoCoroutine({maxTableDepth = 9}, "Reflect",unpack(toSend))

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)

	local ok, str = DeepMatch(toSend,res,10)
	if  ok then
		return true
	else
		error("Mismatch at " .. str .. " :: " .. obj2str(res))
	end
end 

-- Nested tables non-tree, diferent depth
Step5 = function()

	local toSend = {toSend}
	toSend[1] = toSend
	
	T1 = w:DoCoroutine({maxTableDepth = 9}, "Reflect",unpack(toSend))

	_, res= T1:Await(100)
	
	RaiseFirstWorkerError(w)

	if res == "AnotherResult" then
		return true
	else
		error("Second result was: " .. res)
	end
end 
