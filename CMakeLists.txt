cmake_minimum_required (VERSION 2.6)

project(mcpelauncherdcblock LANGUAGES C)

set(CMAKE_C_STANDARD 17)

find_library(log-lib log android-lib android)

add_library(mcpelauncherdcblock SHARED src/main.c src/main.h src/imgui.c src/imgui.h)
target_link_libraries(mcpelauncherdcblock PUBLIC ${log-lib} android)
