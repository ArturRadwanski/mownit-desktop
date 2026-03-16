cmake_minimum_required(VERSION 4.1)
project(untitle)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/qt6")
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        REQUIRED)

add_executable(main main.cpp
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        qcustomplot.h
        qcustomplot.cpp
        Ui_MainWindow.h)

target_link_libraries(main
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::PrintSupport
)


