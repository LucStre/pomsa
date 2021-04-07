cmake_minimum_required(VERSION 3.9)

#set the project name and version 
project(pomsa VERSION 0.0.0)

configure_file(include/PomsaConfig.h.in PomsaConfig.h)

#specify the c++ standard
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} src/pomsa.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            )

