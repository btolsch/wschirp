cmake_minimum_required(VERSION 3.5)
project(wschirp)

add_subdirectory(libs/i3ipcpp)

include_directories(${I3IPCpp_INCLUDE_DIRS})
link_directories(${I3IPCpp_LIBRARY_DIRS})

set(SOURCES src/main.cpp)

add_executable(wschirp ${SOURCES})
target_compile_options(wschirp
    PUBLIC -std=c++1z -Wall -Wextra -Werror -fcolor-diagnostics)
target_link_libraries(wschirp ${I3IPCpp_LIBRARIES})
