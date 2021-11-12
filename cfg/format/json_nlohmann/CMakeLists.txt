cmake_minimum_required(VERSION 3.20)
project(cfg_json_nlohmann)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE json_nlohmann.hpp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)
