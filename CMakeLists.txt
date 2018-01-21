cmake_minimum_required(VERSION 3.10)
project(wschirp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(libs/i3ipcpp)

include_directories(${I3IPCpp_INCLUDE_DIRS})
link_directories(${I3IPCpp_LIBRARY_DIRS})

add_executable(wschirp src/main.cpp)
target_compile_options(wschirp
    PRIVATE -std=c++1z -Wall -Wextra -Werror -fcolor-diagnostics)
target_link_libraries(wschirp ${I3IPCpp_LIBRARIES})

add_executable(wsfilter src/filter_main.cpp)
target_compile_options(wsfilter
    PRIVATE -std=c++1z -Wall -Wextra -Werror -fcolor-diagnostics)
target_link_libraries(wsfilter ${I3IPCpp_LIBRARIES})

add_executable(nbline src/nbline.cpp)
target_compile_options(nbline
    PRIVATE -std=c++1z -Wall -Wextra -Werror -fcolor-diagnostics)

install(TARGETS wschirp wsfilter nbline RUNTIME DESTINATION bin)
