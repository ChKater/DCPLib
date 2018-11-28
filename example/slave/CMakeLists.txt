cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)

project(dcpslave)
    
add_definitions("-DDEBUG")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

find_package(DCPLib REQUIRED)
find_package(Threads REQUIRED)

include_directories("src")
    
file(GLOB_RECURSE dcpslave_src src/*.cpp src/*.hpp)
add_executable(dcpslave ${dcpslave_src})
target_link_libraries(dcpslave DCPLib::Ethernet DCPLib::Slave)


install(TARGETS dcpslave DESTINATION bin)

