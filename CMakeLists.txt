cmake_minimum_required(VERSION 3.9)

#set the project name and version 
project(pomsa VERSION 0.2.0)

configure_file(include/PomsaConfig.h.in PomsaConfig.h)

#specify the c++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ALIGN_INCLUDES ${PROJECT_SOURCE_DIR}/../include/align)
set(GRAPH_INCLUDES ${PROJECT_SOURCE_DIR}/../include/graph)

include_directories(${ALIGN_INCLUDES})
include_directories(${GRAPH_INCLUDES})

add_executable(${PROJECT_NAME} src/pomsa.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            )

