cmake_minimum_required(VERSION 3.16)

project(glia_app VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/app/main.cpp
    src/config/config.cpp
    src/harvest/harvester.cpp
    src/prune/pruner.cpp
    src/report/reporter.cpp
    src/audit/audit.cpp
    src/cli/cli.cpp
    src/storage/storage.cpp
)

add_executable(sorrel ${SOURCES})

# SDD Test binary
add_executable(sorrel_test
    tests/sdd/cards/GliaAppClass.cpp
    tests/sdd/cards/EnhancementsClass.cpp
    src/config/config.cpp
    src/harvest/harvester.cpp
    src/prune/pruner.cpp
    src/report/reporter.cpp
    src/audit/audit.cpp
    src/cli/cli.cpp
    src/storage/storage.cpp
)
target_include_directories(sorrel_test PRIVATE src tests/sdd/cpp tests/sdd/cards)

# Custom clean target
set(CLEAN_FILES
    "${CMAKE_SOURCE_DIR}/test_out.txt"
    "${CMAKE_SOURCE_DIR}/test_output_1.txt"
    "${CMAKE_SOURCE_DIR}/test_output_2.txt"
    "${CMAKE_SOURCE_DIR}/test_out_final.txt"
    "${CMAKE_SOURCE_DIR}/build_output.txt"
)

add_custom_target(clean_artifacts
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CLEAN_FILES}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/scripts/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/tests/__pycache__"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Removing temporary logs and Python caches"
)
