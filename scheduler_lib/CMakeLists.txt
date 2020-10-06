cmake_minimum_required (VERSION 3.8)

add_library(scheduler_lib 
	schedule_funcs.cpp 
	schedule_funcs.h "structs.h")

target_include_directories(scheduler_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")