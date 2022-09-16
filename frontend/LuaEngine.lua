function getFuncParamsPointer(paramPointer, n)
    return paramPointer + (n - 1) * 4
end

function EnterMessageBoxA(hWnd_p, lpText_p, lpCaption_p, uType_p)
    print("EnterMessageBoxA")
    print(luaGetCStringByPointer(luaGetPointerByPointer(lpText_p)))
    print(luaGetCStringByPointer(luaGetPointerByPointer(lpCaption_p)))
    local newCaption = "hacked"
    local newText = "hacked by lua!"
    local newCaptionPointer = luaNewCString(newCaption)
    local newTextPointer = luaNewCString(newText)
    luaSetPointerByPointer(lpText_p, newCaptionPointer)
    luaSetPointerByPointer(lpCaption_p, newTextPointer)
end

function LeaveMessageBoxA(resultPointer)
    print("LeaveMessageBoxA")
    print(luaGetIntByPointer(resultPointer))
end