cmake_minimum_required(VERSION 3.10)
project(exspdlog)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR ON) 
include_directories("${CMAKE_SOURCE_DIR}/exspdlog")
add_executable("test" "${CMAKE_SOURCE_DIR}/test.cpp")
#add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

