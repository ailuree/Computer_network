cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

SET(PROJECT_NAME catch)
project(${PROJECT_NAME})

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
) 

# Windows特定设置
if(WIN32)
    # 链接WinSock2库
    link_libraries(wsock32 ws2_32)
    
    # 添加Windows特定的编译选项
    add_definitions(
        -D_WIN32_WINNT=0x0600  # 指定Windows版本（Vista及以上）
        -DWIN32_LEAN_AND_MEAN  # 减少Windows头文件包含
    )
endif()

# 添加编码设置
if(MSVC)
    add_compile_options(/utf-8)
endif()

aux_source_directory(src DIR_SRCS)
MESSAGE(STATUS "Src file: ${DIR_SRCS}")

add_executable(${PROJECT_NAME}
    catch.cpp
    ${DIR_SRCS}
) 