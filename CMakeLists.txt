cmake_minimum_required(VERSION 3.16)
project(cfg)

set(CMAKE_CXX_STANDARD 20)

set(${PROJECT_NAME}_CORE_HEADERS
        cfg/option.hpp
        cfg/section.hpp
        cfg/parser.hpp
        cfg/configuration.hpp

        cfg/helper/reflection.hpp

        cfg/type_conversions/chrono_durations.hpp
        )

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)

option(CFG_USE_JSON_NLOHMANN "Use the nlohmann json library" True)
if (${CFG_USE_JSON_NLOHMANN})
    add_subdirectory(cfg/format/json_nlohmann)
    target_link_libraries(${PROJECT_NAME} INTERFACE cfg_json_nlohmann)
endif()

