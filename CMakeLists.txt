cmake_minimum_required (VERSION 2.6)

project(mcpelauncherdcblock LANGUAGES C)

set(CMAKE_C_STANDARD 17)

find_library(log-lib log)


add_library(mcpelauncherdcblock SHARED src/main.c)
target_link_libraries(mcpelauncherdcblock PUBLIC ${log-lib})
