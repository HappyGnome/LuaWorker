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

package.cpath = package.cpath..";"..RootDir.."\\?.dll;"

require('LuaWorker')

-----------------------------------------
RaiseFirstWorkerError = function(worker)
	while (true) do
		local s, l = worker:PopLogLine()
		if l == nil then break end
		if l == LuaWorker.LogLevel.Error then
			error(s)
		end
	end
end
----------------------------------------


DeepMatch = function(t,s,maxDepth)
	if type(maxDepth) ~= 'number' then maxDepth = 10 end
	if maxDepth <= 0  then return false end

	if type(t) ~= type(s) then return false, type(s) end

	if type(s) == 'boolean' and s ~= t then 
		if (s) then
			return false,"true"
		else
			return false, "false"
		end
	end

	if type(t) ~= 'table' then return s == t, s end

	for k,v in pairs(t) do
		local ok, str = DeepMatch(v,s[k], maxDepth -1) 
		if not ok then return false, k .. "/" .. str end
	end

	for k,v in pairs(s) do
		
		local ok, str = DeepMatch(v,t[k], maxDepth - 1) 
		if not ok then return false, k .. ":" .. str	end
	end

	return true
end

------------------------------------
--[[
Basic recursive Lua serializer
(serializeable values are strings, numbers, or tables containing serializeable values)
--]]
obj2str = function(obj, maxDepth)
	if type(maxDepth) ~= 'number' then maxDepth = 10 end
	if maxDepth <= 0 then return "..." end
    if obj == nil then
        return 'nil'
    end
    local msg = ''
    local t = type(obj)
    if t == 'table' then
        msg = msg .. '{'
        for k, v in pairs(obj) do
            msg = msg .. k .. ':' .. obj2str(v,maxDepth -1) .. ', '
        end
        msg = msg .. '}'
    elseif t == 'string' then
        msg = msg .. "\"" .. obj .. "\""
    elseif t == 'number' or t == 'boolean' then
        msg = msg .. tostring(obj)
    elseif t then
        msg = msg .. t
    end
    return msg
end
