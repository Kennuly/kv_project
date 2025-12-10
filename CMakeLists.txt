cmake_minimum_required(VERSION 3.10)    #指定 CMake 的最低版本
project(kv_project)     #声明一个项目

set(CMAKE_CXX_STANDARD 17)  #设置 C++ 标准
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(SRC src/main.cpp)       #定义源文件变量     给变量 SRC 赋值为你的代码路径。

include_directories(${CMAKE_SOURCE_DIR}/include)

# 服务端程序
add_executable(kv_server
    src/main.cpp
    src/TcpServer.cpp
    src/TcpConnection.cpp
)

# 客户端程序
add_executable(kv_client
    src/client.cpp
)
