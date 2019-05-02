# Find includes in corresponding build directories
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(GUI_PoliTOcean LANGUAGES CXX)

# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

# need this to include the auto generated ui_mainwindow.h
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources
    main.cpp
    src/mainwindow.cpp
    src/autodrive.cpp
    src/vision.cpp
    src/messages_publisher.cpp
    src/rovcontrol.cpp)


find_package(OpenCV REQUIRED)

add_executable(gui_politocean
        ${project_sources}
        ${ui_wrap}
        ${moc_sources})

target_link_libraries(gui_politocean
        PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        -lpaho-mqttpp3
        ${OpenCV_LIBS})
