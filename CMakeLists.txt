cmake_minimum_required(VERSION 3.16)

project(RetailMS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)
qt_standard_project_setup()

add_executable(RetailMS
    src/main.cpp
    src/core/product.h
    src/core/salesorder.h
    src/core/inventoryrepository.h
    src/core/inventoryrepository.cpp
    src/ui/producttablemodel.h
    src/ui/producttablemodel.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.cpp
)

target_link_libraries(RetailMS
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)

if(WIN32)
    set_target_properties(RetailMS PROPERTIES WIN32_EXECUTABLE ON)
endif()
