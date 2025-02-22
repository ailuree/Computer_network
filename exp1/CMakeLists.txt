cmake_minimum_required(VERSION 3.10)
project(packet_capture)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(packet_capture code/main.cpp)

# Windows特定设置
if(WIN32)
    # 链接WinSock2库
    target_link_libraries(packet_capture wsock32 ws2_32)
    
    # 如果需要，可以添加Windows特定的编译选项
    target_compile_definitions(packet_capture PRIVATE
        _WIN32_WINNT=0x0600  # 指定Windows版本（Vista及以上）
        WIN32_LEAN_AND_MEAN  # 减少Windows头文件包含
    )
endif()

# 设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)