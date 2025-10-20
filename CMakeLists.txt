cmake_minimum_required(VERSION 3.10)
project(Socket VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Debug)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 libevent
find_package(Libevent CONFIG REQUIRED)

# 添加可执行文件（一次性列出所有源文件）
add_executable(Socket
    main.cpp
    Network/NetworkModule.cpp
    Service/ServiceModule.cpp
    Protocol/ProtocolModule.cpp
    Protocol/Reassembler.cpp
    Protocol/MultiUserReassembler.cpp
    
)

# 头文件搜索路径
target_include_directories(Socket PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Network
    ${CMAKE_CURRENT_SOURCE_DIR}/Service
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol
)

# 链接 libevent 库
target_link_libraries(Socket PRIVATE
    libevent::core
    libevent::extra
    libevent::pthreads
)
