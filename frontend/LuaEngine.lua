function EnterMessageBoxA(hWnd_p, lpText_p, lpCaption_p, uType_p)
    print("EnterMessageBoxA")
    print(luaGetCStringByPointer(luaGetPointerByPointer(lpText_p)))
    print(luaGetCStringByPointer(luaGetPointerByPointer(lpCaption_p)))
    local newCaption = "hacked"
    local newText = "hacked by lua!!!"
    local newCaptionPointer = luaNewCString(newCaption)
    local newTextPointer = luaNewCString(newText)
    luaSetPointerByPointer(lpText_p, newTextPointer)
    luaSetPointerByPointer(lpCaption_p, newCaptionPointer)
end

function LeaveMessageBoxA(resultPointer)
    print("LeaveMessageBoxA")
    print(luaGetIntByPointer(resultPointer))
end