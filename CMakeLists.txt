# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Scheduler")

# Include sub-projects.
add_subdirectory ("scheduler_lib")
add_subdirectory ("Scheduler")
add_subdirectory("test")

