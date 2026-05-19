cmake_minimum_required(VERSION 3.16)

project(sorrel VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Xml)

# Auto-generate MOC files
set(CMAKE_AUTOMOC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/glia_config.cpp
    src/glia_config_manager.cpp
    src/harvester.cpp
    src/pruner.cpp
    src/reporting.cpp
)

# Headers
set(HEADERS
    src/glia_config.h
    src/glia_config_manager.h
    src/harvester.h
    src/pruner.h
    src/reporting.h
)

# Main Application binary
add_executable(sorrel ${SOURCES} ${HEADERS})
target_link_libraries(sorrel PRIVATE Qt6::Core Qt6::Xml)

# SDD Test binary
add_executable(sorrel_test
    tests/sdd/cards/GliaAppClass.cpp
    tests/sdd/cards/EnhancementsClass.cpp
    src/glia_config.cpp
    src/glia_config_manager.cpp
    src/harvester.cpp
    src/pruner.cpp
    src/reporting.cpp
)
target_include_directories(sorrel_test PRIVATE src tests/sdd/cpp)
target_link_libraries(sorrel_test PRIVATE Qt6::Core Qt6::Xml)

# Custom clean target to remove artifacts
set(CLEAN_FILES
    "${CMAKE_SOURCE_DIR}/test_out.txt"
    "${CMAKE_SOURCE_DIR}/test_output_1.txt"
    "${CMAKE_SOURCE_DIR}/test_output_2.txt"
    "${CMAKE_SOURCE_DIR}/test_out_final.txt"
    "${CMAKE_SOURCE_DIR}/build_output.txt"
)

add_custom_target(clean_artifacts
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CLEAN_FILES}
    # Portable Python cleanup
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/scripts/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/tests/__pycache__"
    # Note: For full cleanup including binaries, the user should remove the build directory.
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Removing temporary logs and Python caches"
)
