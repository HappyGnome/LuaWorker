
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


Echo = function(n,...)

	local s = ""

	if n == nil then return "NIL" end

	for k = 1,n do
		local v = arg[k]
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
end]]

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoString(initStr)

	RaiseFirstWorkerError(w)
	
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2 = function()

	T1 = w:DoCoroutine("Echo",2,"a",1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1a21000" then
		return true
	else
		error(res)
	end
end 

Step3 = function()

	T1 = w:DoCoroutine("Echo",2,"",1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "121000" then
		return true
	else
		error(res)
	end
end 

Step4 = function()

	T1 = w:DoCoroutine("Echo",4,10,"",nil,1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "110241000" then
		return true
	else
		error(res)
	end
end 

Step5 = function()

	T1 = w:DoCoroutine("Echo",4,10,"\0Hello\0",nil,1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)

	local tgt = "1102\0Hello\0".."41000"

	if res == tgt then
		return true
	else
		error(res)
	end

	return true
end 


Step6 = function()

	T1 = w:DoCoroutine("Echo",4,true,"\n",nil,1000.645)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1true2\n41000.645" then
		return true
	else
		error(res)
	end
end 

Step7 = function()

	T1 = w:DoCoroutine("Echo",0)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "" then
		return true
	else
		error(res)
	end
end 
Step8 = function()

	T1 = w:DoCoroutine("Echo",1,false)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1false" then
		return true
	else
		error(res)
	end
end 

-- Empty arg bundle
Step9 = function()

	T1 = w:DoCoroutine("Echo")

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "NIL" then
		return true
	else
		error(res)
	end
end 

