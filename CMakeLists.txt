cmake_minimum_required(VERSION 3.16)

project(glia_app VERSION 1.0.0 LANGUAGES CXX)

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
)

# Headers
set(HEADERS
    src/glia_config.h
    src/glia_config_manager.h
)

# Application binary
add_executable(glia_app_bin ${SOURCES} ${HEADERS})
target_link_libraries(glia_app_bin PRIVATE Qt6::Core Qt6::Xml)

# Test binary (including SORREL card for verification)
add_executable(glia_app_test
    tests/sdd/cards/GliaAppClass.cpp
    src/glia_config.cpp
    src/glia_config_manager.cpp
)
target_include_directories(glia_app_test PRIVATE src tests/sdd/cpp)
target_link_libraries(glia_app_test PRIVATE Qt6::Core Qt6::Xml)
