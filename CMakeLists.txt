cmake_minimum_required (VERSION 2.6)

project(mcpelauncherdcblock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_library(log-lib log android-lib android)

add_subdirectory(properties-parser)

add_library(mcpelauncherdcblock SHARED src/main.cpp src/conf.cpp src/conf.h src/dcblock.cpp src/dcblock.h src/imgui.cpp src/imgui.h src/util.cpp src/util.h)
target_link_libraries(mcpelauncherdcblock PUBLIC ${log-lib} android properties-parser)
