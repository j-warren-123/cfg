cmake_minimum_required(VERSION 3.20)
project(cfg)

set(CMAKE_CXX_STANDARD 20)

set(${PROJECT_NAME}_CORE_HEADERS
        cfg/option.hpp
        cfg/section.hpp
        cfg/parser_generator.hpp
        cfg/helper/common.hpp
        )

set(${PROJECT_NAME}_OPTIONAL_FORMAT
        cfg/format/json.hpp
        cfg/helper/reflection.hpp
        )

add_library(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_CORE_HEADERS} ${${PROJECT_NAME}_OPTIONAL_FORMAT})

target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR})

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)

