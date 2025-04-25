
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


Echo = function(ch, ...)

	local s = ""
	for k,v in ipairs(arg) do
		if type(v) == 'boolean' then
			if v then
				v = "true"
			else
				v= "false"
			end
		end
		s = s .. k .. v
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

	T1 = w:DoCoroutine("Echo","a",1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1a21000" then
		return true
	else
		error(res)
	end
end 

Step3 = function()

	T1 = w:DoCoroutine("Echo","",1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "121000" then
		return true
	else
		error(res)
	end
end 

Step4 = function()

	T1 = w:DoCoroutine("Echo",10,"",nil,1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "110241000" then
		return true
	else
		error(res)
	end
end 

Step5 = function()

	T1 = w:DoCoroutine("Echo",10,"\0Hello\0",nil,1000)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1102\0Hello\041000" then
		return true
	else
		error(res)
	end
end 


Step5 = function()

	T1 = w:DoCoroutine("Echo",true,"\n",nil,1000.645)

	res = T1:Await(100)
	
	RaiseFirstWorkerError(w)
	
	if res == "1true2\n41000.645" then
		return true
	else
		error(res)
	end
end 
