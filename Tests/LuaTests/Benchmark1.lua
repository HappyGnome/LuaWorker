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
Echo = function(...)
	return arg
end
]]

w = LuaWorker.Create()
w:Start()

Step1 = function()
	w:DoString(initStr):Await(500)

	RaiseFirstWorkerError(w)
	
	return w:Status() == LuaWorker.WorkerStatus.Processing
end 

Step2NoData = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo")
		w:DoCoroutine("Echo")
		w:DoCoroutine("Echo")
		lastTask = w:DoCoroutine("Echo")
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

Step2Int = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo",1)
		w:DoCoroutine("Echo",2)
		w:DoCoroutine("Echo",3)
		lastTask = w:DoCoroutine("Echo",4)
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

Step2String = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo","Here is a string 1234567890124124124 Some random stu f:f 90u1jnwuehrp2983y4ph34lc2u3h5p29n83yc5[qc283y4c2 34h;2u5h232[034y 2q;3hr ;2uh234nuc3[9u'2oh4/']]]")
		w:DoCoroutine("Echo","Here is a string 1234567890124124124 Some random stu f:f 90u1jnwuehrp2983y4ph34lc2u3h5p29n83yc5[qc283y4c2 34h;2u5h232[034y 2q;3hr ;2uh234nuc3[9u'2oh4/']]]")
		w:DoCoroutine("Echo","Here is a string 1234567890124124124 Some random stu f:f 90u1jnwuehrp2983y4ph34lc2u3h5p29n83yc5[qc283y4c2 34h;2u5h232[034y 2q;3hr ;2uh234nuc3[9u'2oh4/']]]")
		lastTask = w:DoCoroutine("Echo","Here is a string 1234567890124124124 Some random stu f:f 90u1jnwuehrp2983y4ph34lc2u3h5p29n83yc5[qc283y4c2 34h;2u5h232[034y 2q;3hr ;2uh234nuc3[9u'2oh4/']]]")
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

Step2Bool = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo",true)
		w:DoCoroutine("Echo",false)
		w:DoCoroutine("Echo",true)
		lastTask = w:DoCoroutine("Echo",false)
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

Step2Table1 = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo",{1})
		w:DoCoroutine("Echo",{true})
		w:DoCoroutine("Echo",{"Hi"})
		lastTask = w:DoCoroutine("Echo",{})
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

Step2Table2 = function()

	local RepCount = 250000
	local lastTask = nil

	for i = 1,RepCount do
		w:DoCoroutine("Echo",{1, {{"124124"}}})
		w:DoCoroutine("Echo",{true, {{"124124"}}})
		w:DoCoroutine("Echo",{"Hi"}, {{"124124"}})
		lastTask = w:DoCoroutine("Echo",{{{"124124"}}})
	end

	local done = lastTask:Await(60000)

	RaiseFirstWorkerError(w)

	return done ~= nil
end 

