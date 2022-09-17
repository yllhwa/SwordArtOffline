wails build -clean -upx -webview2 error
@REM 将HookDemo.dll和Injector.exe放到build/bin目录下
copy HookDemo.dll build\bin
copy Injector.exe build\bin
copy LuaEngine.lua build\bin