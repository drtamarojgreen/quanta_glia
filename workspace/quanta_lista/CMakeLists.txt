cmake_minimum_required(VERSION 3.10)

project(quanta_lista LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components
find_package(Qt6 COMPONENTS Widgets Xml REQUIRED)
# Fallback to Qt5 if Qt6 is not found
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Xml REQUIRED)
endif()

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(quanta_lista
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/task.h
    src/task_manager.cpp
    src/task_manager.h
    src/config_manager.cpp
    src/config_manager.h
)

if(Qt6_FOUND)
    target_link_libraries(quanta_lista PRIVATE Qt6::Widgets Qt6::Xml)
else()
    target_link_libraries(quanta_lista PRIVATE Qt5::Widgets Qt5::Xml)
endif()
