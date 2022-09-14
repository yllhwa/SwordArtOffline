cmake_minimum_required(VERSION 3.21)
project(HookDemo)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_CL_64)
    set(CURRENT_PLATFORM "x64")
    message(STATUS "Current Platform is ${CURRENT_PLATFORM}")
else(CMAKE_CL_64)
    set(CURRENT_PLATFORM "x86")
    message(STATUS "Current Platform is ${CURRENT_PLATFORM}")
endif(CMAKE_CL_64)

include_directories(include)
if(CURRENT_PLATFORM MATCHES "x86")
    link_directories("lib.X86")
endif()

link_libraries(detours.lib Ws2_32.lib)

add_library(HookDemo SHARED main.cpp UDPClient/UDPClient.cpp UDPClient/UDPClient.h Base64/base64.cpp Base64/base64.h Locale/Locale.cpp Locale/Locale.h MD5/md5.cpp MD5/md5.h Utils/utils.cpp Utils/utils.h)

add_executable(Injector Injector/Injector.cpp)

add_custom_command(TARGET HookDemo POST_BUILD
        COMMAND echo "Copy target to frontend"
        COMMAND ${CMAKE_COMMAND} -E copy HookDemo.dll ../../frontend/HookDemo.dll
        COMMAND ${CMAKE_COMMAND} -E copy Injector.exe ../../frontend/Injector.exe
        )