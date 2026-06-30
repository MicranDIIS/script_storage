-----------------------------------------------------------------------------
-- Declare module and import dependencies
-----------------------------------------------------------------------------

local  bbase = _G
local ldap = bbase.require("lualdap")
module("base")
local devco_path = "start \\\\k118\\Pribor\\TheSOFT\\deVco\\deco.exe "
-- local m = {}

bbase.cc_exec = 0
dataTable = {}
stateReboot = false
blocking = 1

local RED = 3
local GREEN = 2
local BLUE = 1

function deepcopy(orig)
    local orig_type = bbase.type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in bbase.next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        bbase.setmetatable(copy, deepcopy(bbase.getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end


function findTagBlocks(text, tag, start, stop)
	local result = {}
	local tagOpen = "<"..tag..">"
	local tagClose = "</"..tag..">"
	local initIndex = start
	local isRun = true
	local startIndex, stopIndex = -1
	while (isRun) do
		local openSIndex, openFIndex = bbase.string.find(text, tagOpen, initIndex, true)
		local closeSIndex, closeFIndex = bbase.string.find(text, tagClose, initIndex, true)
		if (openSIndex and openFIndex and closeSIndex and closeFIndex) then
			local block = bbase.string.sub (text, openFIndex + 1, closeSIndex - 1)
			initIndex = closeFIndex
			if (initIndex > stop) then
				isRun = false
			else
				local findIndexes = {}
				bbase.table.insert(findIndexes, openFIndex + 1)
				bbase.table.insert(findIndexes, closeSIndex - 1)
				bbase.table.insert(result, findIndexes)
			end
		else
			isRun = false
		end
	end
	return result
end

function GetXmlTagValue(dataString, tagsList)
	local tags = {}
	local result = ""
	ParseStringSimple(tagsList, tags, false)
	local indexArray = {{1, bbase.string.len(dataString)}}
	for i = 1, #tags, 1 do
		local newIndexArray = {}
		for j = 1, #indexArray, 1 do
			--secretLogPointer:addRecord("Find "..tags[i])
			local findBlocksIndex = findTagBlocks(dataString, tags[i], indexArray[j][1], indexArray[j][2])
			for k = 1, #findBlocksIndex, 1 do
				--secretLogPointer:addRecord(tags[i]..":"..#findBlocksIndex)
				bbase.table.insert(newIndexArray, findBlocksIndex[k])
			end
		end
		indexArray = deepcopy(newIndexArray)
		if (i == #tags) then
			if #indexArray == 0 then return "error" end
			--secretLogPointer:addRecord("indexS:"..(#indexArray))--.."; indexE:"..indexArray[1][2] + 1)
			return bbase.string.sub (dataString, indexArray[1][1], indexArray[1][2])
		end
	end
	return "empty"
end

function ChangeXmlTagValue(fileName, tagsList, newValue, isReplace)
	local tags = {}
	local result = ""
	ParseStringSimple(tagsList, tags, false)
	local file = bbase.io.open(fileName, "r")
	local fileContent = file:read("*a")
	file:close()
	local indexArray = {{1, bbase.string.len(fileContent)}}
	for i = 1, #tags, 1 do
		local newIndexArray = {}
		for j = 1, #indexArray, 1 do
			local findBlocksIndex = findTagBlocks(fileContent, tags[i], indexArray[j][1], indexArray[j][2])
			for k = 1, #findBlocksIndex, 1 do
				bbase.table.insert(newIndexArray, findBlocksIndex[k])
			end
		end
		indexArray = deepcopy(newIndexArray)
		if (i == #tags) then
			local firstPart = bbase.string.sub (fileContent, 1, indexArray[1][1] - 1)
			local secondPart = bbase.string.sub (fileContent, indexArray[1][2] + 1, bbase.string.len(fileContent))
			result = firstPart..newValue..secondPart
			if (isReplace) then
				local file = bbase.io.open(fileName, "w")
				local fileContent = file:write(result)
				file:close()
			end
		end
	end
	return result
end

function ChangeXmlTagValueInString(data, tagsList, newValue)
	local tags = {}
	local result = ""
	ParseStringSimple(tagsList, tags, false)
	--local fileContent = data
	local indexArray = {{1, bbase.string.len(data)}}
	for i = 1, #tags, 1 do
		local newIndexArray = {}
		for j = 1, #indexArray, 1 do
			local findBlocksIndex = findTagBlocks(data, tags[i], indexArray[j][1], indexArray[j][2])
			for k = 1, #findBlocksIndex, 1 do
				bbase.table.insert(newIndexArray, findBlocksIndex[k])
			end
		end
		indexArray = deepcopy(newIndexArray)
		if (i == #tags) then
			local firstPart = bbase.string.sub (data, 1, indexArray[1][1] - 1)
			local secondPart = bbase.string.sub (data, indexArray[1][2] + 1, bbase.string.len(data))
			result = firstPart..newValue..secondPart
		end
	end
	return result
end

function getHexColor(num, color)
	local hexstr = '0123456789abcdef'
	local s = ''
	if (num ~= 0) then
		num = bbase.tonumber(num)
		while num > 0 do
			local modd = bbase.math.fmod(num, 16)
			s = bbase.string.sub(hexstr, modd+1, modd+1) .. s
			num = bbase.math.floor(num / 16)
		end
	else
		s = "0"
	end
	local length = bbase.string.len(s)
	if (length < 2) then s = "0"..s end

	if (color == RED) then
		s = "0x"..s.."0000"
	elseif (color == GREEN) then
		s = "0x00"..s.."00"
	elseif (color == BLUE) then
		s = "0x0000"..s
	end

	return s
end

function GetColors(number)
	local alphabet = bbase.math.ceil(number^(1/3))
	local start_color = 0
	local stop_color = 0xbf
	local step = (stop_color - start_color) / (alphabet - 1)

	local red = {}
	local green = {}
	local blue = {}

	for i = 0, alphabet - 1, 1 do
		bbase.table.insert(red, getHexColor(bbase.math.floor(start_color + step * i), RED))
		bbase.table.insert(green, getHexColor(bbase.math.floor(start_color + step * i), GREEN))
		bbase.table.insert(blue, getHexColor(bbase.math.floor(start_color + step * i), BLUE))
	end

	local all_colors = {}

	for i = 1, #red, 1 do
		for j = 1, #green, 1 do
			for k = 1, #blue, 1 do
				local one_color = bbase.tonumber(red[i], 16) + bbase.tonumber(green[j], 16) + bbase.tonumber(blue[k], 16)				
				bbase.table.insert(all_colors, one_color)
			end
		end
	end
	
	local result = {}
	local counter = 1
	for i = 1, number, 1 do
		if (i % 2 == 1) then
			bbase.table.insert(result, all_colors[counter])	
		else
			bbase.table.insert(result, all_colors[#all_colors - counter + 1])
			counter = counter + 1
		end
	end	
	return result
end

function IsDirExist(dirName)
    local result
	if bbase.type(dirName)~="string" then
		return false
	end
	if bbase.os.rename(dirName,dirName) then
		result = true
    else
		result = false
	end
    return result
end

function GetTime(exec_seconds)
	local minutes = bbase.math.floor(exec_seconds/60)
	local seconds = exec_seconds - minutes * 60
	seconds = bbase.tostring(seconds)
	if (bbase.string.len(seconds) == 1) then
		seconds = "0"..seconds
	end
	local result = minutes..":"..seconds
	return result
end

function GetUserSurname(user, password)
	local result = nil
	local connectParrams = "uid="..user..",ou=People,dc=localnet,dc=micran"
	local ldapServer = ldap.open_simple ("ldap.micran.ru:389", connectParrams, password)
	if (ldapServer == nil) then
		if secretLogPointer then
			secretLogPointer:addRecord("Не удалось подключиться к серверу по ldap")
		end
		result = nil
	else
		for dn, attribs in ldapServer:search { base = "uid="..user..",ou=People,dc=localnet,dc=micran" } do
			for name, values in bbase.pairs (attribs) do
				if (name == "sn") then
					result = values
				end
			end
		end
	end
	return result
end

--Распознавание параметров калибровки
function GetCalibrSteps(allRarams, resultTable)
	local start = 1
	local stepNumber = 1
	local part = 1
	resultTable[stepNumber] = {}
	resultTable[stepNumber][part] = {}

	for i = 1, #allRarams do
		if allRarams:sub(i,i) == "*" then
			bbase.table.insert(resultTable[stepNumber][part], allRarams:sub(start, i-1))
			stepNumber = stepNumber + 1
			resultTable[stepNumber] = {}
			part = 1
			resultTable[stepNumber][part] = {}
			start = i+1

		end
		if allRarams:sub(i,i) == " " then
			bbase.table.insert(resultTable[stepNumber][part], allRarams:sub(start, i-1))
			start = i+1
		end
		if allRarams:sub(i,i) == "%" then
			bbase.table.insert(resultTable[stepNumber][part], allRarams:sub(start, i-1))
			part = part + 1
			resultTable[stepNumber][part] = {}
			start = i+1
		end
	end
	bbase.table.remove(resultTable,#resultTable)
end

function SmoothData(data, percent)
	local result = ""
	if (percent > 30) then
		return result
	end
	
	local Points = {}
	local X = {}
	local Y = {}
	local SmoothY = {}
	if(bbase.type(data)=="string") then
		ParseString(data, Points)
	else
		Points = data
	end

	for i = 1, #Points, 1 do
		if ((Points[i][1] ~= nil) and (Points[i][2] ~= nil)) then
			bbase.table.insert(X, Points[i][1])
			bbase.table.insert(Y, Points[i][2])
		end
	end

	local pointNumber = #Y
	local onePercent = pointNumber/100
	local averageWindowSize = math_round(onePercent * percent,0)
	if (averageWindowSize < 2) then averageWindowSize = 2 end
	local halfWindowSize = math_round(averageWindowSize/2, 0)
	for i = 1, #Y, 1 do
		local currentWidth = halfWindowSize
		local divider = 0
		local store = 0
		local smoothYVal = nil
		while (currentWidth ~= 0) do
			local indexUp = i + currentWidth
			local indexLow = i - currentWidth

			if (indexUp <= pointNumber) then
				store = store + Y[indexUp]
                divider = divider + 1
            end
            if (indexLow >= 1) then
                store = store + Y[indexLow]
                divider = divider + 1
            end
            currentWidth = currentWidth - 1

		end
		newYVal = store / divider
		bbase.table.insert(SmoothY, newYVal)
	end
	for i = 1, #X, 1 do
		result = result..X[i].."\t"..SmoothY[i].."\r\n"
	end
	return result, SmoothY
end

--Проверка на ошибки
function Check(obj, res, name)
	if bbase.abort == 1 then
		--secretLogPointer:addRecord(bbase.type(bbase.FinallyExec))
		bbase.abort = 0
		if bbase.FinallyExec~=nil then bbase.FinallyExec() end
		bbase.error("Прерывание",0)
	end
	if stateReboot == true then return res end
	if res<0 or bbase.abort == 1 then		
		-- if secretLogPointer and blocking == 1 then
			-- local err = obj:GetError()
			-- secretLogPointer:setError(err)
		-- end
		-- local err = obj:GetError()
		-- secretLogPointer:setError(err)
		if blocking == 1 then
			local err = obj:GetError()
			
			if secretLogPointer then
				secretLogPointer:setError(err)
			end
			bbase.error(bbase.tostring(res).." - "..err,0)
		else
			return -1
		end
	--end
	end
	--if bbase.LogText ~= "" then bbase.Log("Ок",1) bbase.LogText = "" end
end

--Функция перезагрузки (только для девайсов поддерживающих СКИП)
function Reboot(mod,host,timeout,dev_name)

	local address = ""
	local s, f = bbase.string.find(host, "::", 1, true)
	if (s == nil) then
		address = "TCPIP::"..host.."::8888::SOCKET"
	else
		address = host
	end

	if not timeout then timeout = 15 end

	if dev_name~=nil then
		if secretLogPointer then
			secretLogPointer:addRecord(dev_name..": Перезагрузка...")
		end
	else
		if secretLogPointer then
			secretLogPointer:addRecord("Перезагрузка...")
		end
	end
	mod:Send("RESET")
	-- blocking = 0
	mod:Send("MODE = 7",false)
	-- blocking = 1
	-- mod:Send("RESET")
	mod:SleepMS(2000)
	r = 1
	k = 0
	blocking = 0
	stateReboot = true
	while r~=0 do
		mod:SleepMS(1000)
		mod:Disconnect()
		r,e = mod:Connect(address)
		if k==timeout then
			stateReboot = false
			blocking = 1
		end
		k = k + 1
	end
	if secretLogPointer then
		secretLogPointer:addRecord("Перезагрузка окончена. Время выполнения: "..k+2 .." c.")
	end
	blocking = 1
	stateReboot = false
end

--Очистка соединений
function ClearConnections()
	if bbase.LModList ~= nil then	
		if secretLogPointer then
			secretLogPointer:addRecord("Закрываю соединения")
		end
		for i = 2,#bbase.LModList,2 do
			local obj = bbase[bbase.LModList[i]]
			if obj["Disconnect"] then obj:Disconnect() end
			if obj["Close"] then obj:Close() end
		end
		if secretLogPointer then
			secretLogPointer:addRecord("")
		end
	end
	bbase.cc_exec = 1
end

function LogMsg()
	if bbase.LogText ~= "" then bbase.Log(bbase.LogText,0)end
end

--Функция регистрации библиотек
function RegModule( modlist )
	for i = 1,#modlist,2 do
		local res_tmp
		local cname_tmp
		res_tmp, cname_tmp = bbase.Registration(modlist[i])
		--if bbase.logWin then bbase.logWin:addRecord(res_tmp) end
		--if bbase.logWin then bbase.logWin:addRecord(cname_tmp) end
		if res_tmp ~= 0 then
			--bbase.Log("Ошибка регистрации модуля "..modlist[i],2)
			bbase.error("Ошибка регистрации модуля "..modlist[i],0)
		end
		local tmp = bbase[cname_tmp]:new()		
		if modlist[i]=="Log.dll" then secretLogPointer = tmp end
		--if bbase.logWin then bbase.logWin:addRecord(bbase.tostring(cname_tmp)) end
		bbase[modlist[i+1]] = tmp
		
		-- local co = 1
		-- if (modlist[i]=="R2G7.dll" and co==1) then 
			-- local tab = bbase.getmetatable(tmp)
			-- for k,v in bbase.pairs(tab) do
				-- secretLogPointer:addRecord(k)
			-- end	
		-- end
		
		
		
		local mt = bbase.getmetatable(tmp)
		--if bbase.logWin then bbase.logWin:addRecord(bbase.type(mt["__index"](tmp,"new"))) end
		for k,v in bbase.pairs(mt) do
			--if bbase.logWin and cname_tmp == "Office" then bbase.logWin:addRecord(bbase.type(v).."  "..k) end
			if k:match("^__") or k:match("^(%.)") or k:match("^GetError") or k:match("^delete") or k:match("^GetAnsw") or k:match("^get_params_list")   or k:match("^tolua") or k:match("^Disconnect") or k:match("^bankObj") or k:match("^GetSN") or k:match("^GetRawData") then
				mt[k] = v
			else
				mt[k] = function(...)
					if arg.n == 0 then return Check(arg[1],v(),cname_tmp)
					elseif arg.n == 1 then return Check(arg[1],v(arg[1]),cname_tmp)
					elseif arg.n == 2 then return Check(arg[1],v(arg[1], arg[2]),cname_tmp)
					elseif arg.n == 3 then return Check(arg[1],v(arg[1], arg[2], arg[3]),cname_tmp)
					elseif arg.n == 4 then return Check(arg[1],v(arg[1], arg[2], arg[3], arg[4]),cname_tmp)
					elseif arg.n == 5 then return Check(arg[1],v(arg[1], arg[2], arg[3], arg[4], arg[5]),cname_tmp)
					elseif arg.n == 6 then return Check(arg[1],v(arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]),cname_tmp)
					elseif arg.n == 7 then return Check(arg[1],v(arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7]),cname_tmp)
					end
					bbase.error("Число параметров в вызываемой функции превышает 6",0)
				end
			end
		end
	end
end

function ParseStringSimple(str,array,notonum)
	for val in bbase.string.gmatch(str, "[^%s\t,]+") do
		if(notonum) then
			bbase.table.insert(array,bbase.tonumber(val))
		else			
			bbase.table.insert(array,val)
		end
    end
end
--функция извлечения цифровых значений из строки
function ParseString(string,array,notonum)
	pt = 0
	pt_next = 0
	cur_row = 1
	cur_col = 1
		if #string > 0 then
		while pt do
			pt_next = string:find ( "\n" , pt+1)
			if pt_next then
				array[cur_row] = {}
				vals = string:sub(pt+1, pt_next )
				for val in string.gmatch(vals, "[^ \t\r\n]+") do
					if(notonum) then
						bbase.table.insert(array[cur_row],val)
					else
						bbase.table.insert(array[cur_row],bbase.tonumber(val))
					end
				end
				cur_row = cur_row+1
				pt = pt_next
			else
				pt_next = -1
				array[cur_row] = {}
				vals = string:sub(pt+1, pt_next )
				for val in string.gmatch(vals, "[^ \t\r\n]+") do
					if(notonum) then
						bbase.table.insert(array[cur_row],val)
					else
						bbase.table.insert(array[cur_row],bbase.tonumber(val))
					end
				end
				return
			end
		end
	end
end

function ParseString2(stringIN,array,notonum)
	pt = 0
	pt_next = 0
	cur_row = 1
	cur_col = 1
		if #stringIN > 0 then
		while pt do
			pt_next = stringIN:find ( "\n" , pt+1)
			if pt_next then
				array[cur_row] = {}
				vals = stringIN:sub(pt+1, pt_next )
				for val in bbase.string.gmatch(stringIN, "[.]+") do
					if(notonum) then
						bbase.table.insert(array[cur_row],val)
					else
						bbase.table.insert(array[cur_row],bbase.tonumber(val))
					end
				end
				cur_row = cur_row+1
				pt = pt_next
			else
				pt_next = -1
				array[cur_row] = {}
				vals = stringIN:sub(pt+1, pt_next )
				for val in bbase.string.gmatch(vals, "[^ \t+\v+\r+\n+\f]+") do
					if(notonum) then
						bbase.table.insert(array[cur_row],val)
					else
						bbase.table.insert(array[cur_row],bbase.tonumber(val))
					end
				end
				return
			end
		end
	end
end

--Тестовая функция для взаимодействия с плагинами
function ServPlugin(...)
	local res
	if arg.n > 0 then
		Check(temporarymodname.Connect(tostring(arg[1])))
		local params = ""
		for i=2,arg.n,1 do
			if i~=2 then params = params.." ," end
			params = params..arg[i]
		end
		Check(temporarymodname.Send("RESET\r\n"))
		if arg.n == 3 then Check(temporarymodname[tostring(arg[2])](arg[3]))
		elseif arg.n == 4 then Check(temporarymodname[tostring(arg[2])](arg[3],arg[4]))
		elseif arg.n == 5 then Check(temporarymodname[tostring(arg[2])](arg[3],arg[4],arg[5]))
		end
		Check(temporarymodname.Disconnect())
	end
	return res
end

--Функция перевода десятеричного числа в шестнацатеричное
function Dec2Hex(nValue)
	if bbase.type(nValue) == "string" then
		nValue = bbase.String.ToNumber(nValue);
	end
	nHexVal = bbase.string.format("%X", nValue);  -- %X returns uppercase hex, %x gives lowercase letters
	sHexVal = nHexVal.."";
	return sHexVal;
end

--Функция округления
function math_round( roundIn , roundDig ) -- первый аргумент - число которое надо округлить, второй аргумент - количество символов после запятой.
        local mul = bbase.math.pow( 10, roundDig )
        return ( bbase.math.floor( ( bbase.tonumber(roundIn) * mul ) + 0.5 )/mul )
end

--Функция открытия и парсинга файла дет. характеристики
function openDetCh( path, skipTo)
	local cntDet = 1
	local noData = 1
	local skipToLocal = 13
	if skipTo then skipToLocal = skipTo end
	for line in bbase.io.lines ( path ) do
		if noData>skipToLocal and noData<453 then
			dataTable[cntDet] = {}
			local lineVal = 0
			pt = 0
			while pt ~= nil do
				lineVal = line:sub (pt+1, line:find ( "\t" , pt+1) )
				lineVal = lineVal:gsub ('^%s*(.-)%s*$', '%1')
				bbase.table.insert ( dataTable[cntDet], bbase.tonumber(lineVal) )
				if line:find ( "\t" , pt+1) ~= nil then
					pt = line:find ( "\t" , pt+1)
				else
					lineVal = line:sub (pt+1, line:find ( "\n" , pt+1) )
					lineVal = lineVal:gsub ('^%s*(.-)%s*$', '%1')
					bbase.table.insert ( dataTable[cntDet], bbase.tonumber(lineVal) )
					break
				end
			end
			cntDet = cntDet + 1
		end
	noData = noData + 1
	end
	--if bbase.LogText ~= "" then bbase.Log("Ок",1) bbase.LogText = "" end
end

--Функция пересчёта мкВ в дБм по Дет. Хар-ке
function CalcDbm ( U, F )
    local Power1
	local Power2
	local Power3
	local Power4
	local F1
	local F2
	local U1
	local U2
	local indF1
	local indF2
	local indU1
	local indU2
	local Pone
	local Ptwo
	local Pres
	for i = 2, #dataTable[1] do
		if(F < dataTable[1][2]) then
			F1 = dataTable[1][2]
			F2 = dataTable[1][2]
			indF1 = 2
			indF2 = 2
			break
		end
		if((F > dataTable[1][i]) and (i == #dataTable[1])) then
			F1 = dataTable[1][#dataTable[1]]
			F2 = dataTable[1][#dataTable[1]]
			indF1 = #dataTable[1]
			indF2 = #dataTable[1]
			break
		end
		if(F < dataTable[1][i]) then
			F1 = dataTable[1][i - 1]
			F2 = dataTable[1][i]
			indF1 = i - 1
			indF2 = i
			break
		end
		if(F ==  dataTable[1][i]) then
			F1 = dataTable[1][i]
			F2 = dataTable[1][i]
			indF1 = i
			indF2 = i
			break
		end
	end

	for j = 2, #dataTable	do
		if(U < dataTable[2][1]) then
			Power1 = dataTable[2][indF1]
			Power2 = dataTable[2][indF2]
			Power3 = dataTable[2][indF1]
			Power4 = dataTable[2][indF2]
			indU1 = 2
			indU2 = 2
			break
		end
		if((U > dataTable[j][1]) and (j == #dataTable)) then
			Power1 = dataTable[#dataTable][indF1]
			Power2 = dataTable[#dataTable][indF2]
			Power3 = dataTable[#dataTable][indF1]
			Power4 = dataTable[#dataTable][indF2]
			indU1 = #dataTable
			indU2 = #dataTable
			break
		end
		if(U < dataTable[j][1]) then
			Power1 = dataTable[j - 1][indF1]
			Power2 = dataTable[j - 1][indF2]
			Power3 = dataTable[j][indF1]
			Power4 = dataTable[j][indF2]
			indU1 = j - 1
			indU2 = j
			break
		end
		if(U == dataTable[j][1]) then
			Power1 = dataTable[j][indF1]
			Power2 = dataTable[j][indF2]
			Power3 = dataTable[j][indF1]
			Power4 = dataTable[j][indF2]
			indU1 = j
			indU2 = j
			break
		end
	end
	if((dataTable[1][indF2] - dataTable[1][indF1]) == 0) then
		Pone = Power1
		Ptwo = Power3
	else
		Pone = ((F - dataTable[1][indF1])*(Power2 - Power1)) / (dataTable[1][indF2] - dataTable[1][indF1]) + Power1
		Ptwo = ((F - dataTable[1][indF1])*(Power4 - Power3)) / (dataTable[1][indF2] - dataTable[1][indF1]) + Power3
	end
	if(((dataTable[indU2][1] - dataTable[indU1][1])) == 0) then
		Pres = Pone
	else
		Pres = ((U - dataTable[indU1][1])*(Ptwo - Pone)) / (dataTable[indU2][1] - dataTable[indU1][1]) + Pone
	end
	return Pres
end

function Interp1d(val)
	local ind0 = 1
	local ind1 = 1
	local find = false
	for i = 1, #dataTable do
		if dataTable[1][1] >= val then
			return dataTable[1][2]
		elseif	dataTable[i][1] > val then
			ind1 = i
			ind0 = i-1
			find = true
			break
		elseif dataTable[i][1] == val then
			return dataTable[i][2]
		end
	end
	if not find then
		return dataTable[#dataTable][2]
	end
	return dataTable[ind0][2] + ((dataTable[ind1][2] - dataTable[ind0][2])*(val - dataTable[ind0][1]))/(dataTable[ind1][1] - dataTable[ind0][1])
end

-- Функция извлечения имени файла(с расширением) из path
function FNfromPath(path)
local pt = 0
	while path:find("/",pt+1)~=nil do
		pt = path:find("/",pt+1)
	end
	return path:sub(pt+1,-1)
end

function CallDevCo(addr)
	bbase.os.execute(devco_path..addr)
end
