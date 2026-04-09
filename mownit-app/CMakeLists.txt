cmake_minimum_required(VERSION 4.1)
project(mownit-desktop)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot-source)

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/qt6")
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        REQUIRED)



add_executable(main main.cpp
        main.cpp
        window/mainwindow.cpp
        window/mainwindow.h
        window/mainwindow.ui
        qcustomplot-source/qcustomplot.h
        qcustomplot-source/qcustomplot.cpp
        PointsModel.cpp
        PointsModel.h
        qcustomplot-source/interpolationDrawing.cpp
)
target_link_libraries(main
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::PrintSupport
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot-source)



add_executable(obliczenia
        tests_main.cpp
)



