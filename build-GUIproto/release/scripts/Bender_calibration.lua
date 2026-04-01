--Specific: Калибровка; Categories: Гибочник;

local base = require("base")
local f = require("functions")

LModList = {
"Log.dll","Log",
"Request.dll","Req",
"SCPIDevs.dll","Bender",
-- "Table.dll", "Tab",
"Chart.dll", "Chart"
}

base.RegModule(LModList)

Req:addRequest("Адрес прибора:",0,"address","SelectDev|VisaAddr")
Req:addRequest("Наименование кабеля:",0,"cableName","")
Req:addRequest("Папка для сохранения:",0,"directory","SelectDir")
-- Req:addRequest("Количество измерений:",4,"nMeasures","minimum:2")

local INVALID_ANGLE_MEASURE = -1

-- Tab:setTableName("Данные калибровки")
-- Tab:setHorizontalHeader(1, "Уставка")
-- Tab:setHorizontalHeader(2, "Измерение")

-- for i = 1, nRows, 1 do
	-- Tab:setCellData(i, 1, i * 10)
-- end

local curve = "Измерения"
Chart:createCurve(curve)
Chart:setColor(curve, 0xff0000)
Chart:setWidth(curve, 1)
Chart:setChartName("Измерения")
Chart:setXLabel("Измерения, градусы")
Chart:setYLabel("Уставка, градусы")


function CheckInputValues()

end

function findSerialFromIdn(idn)
	local result = string.match(idn, "%a+,%a+,(%d+),.+")
	return result
end

function Run()
	
	
end

--! При ошибке и прерывании вызываем функцию
function FinallyExec()

end