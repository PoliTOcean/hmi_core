cmake_minimum_required(VERSION 3.5)
project(Joystick VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

# find Qt5 and required components
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

# need this to include the auto generated ui_mainwindow.h
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(ui_wrap form/design.ui)
qt5_wrap_cpp(moc_sources mainwindow.h my_qlabel.h)

add_library(mainwindow SHARED
        mainwindow.cpp
        my_qlabel.cpp
        my_qlabel.h
        ${ui_wrap}
        ${moc_sources})

add_library(PolitoceanHmi::mainwindow ALIAS mainwindow)

target_include_directories(mainwindow
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mainwindow
                                PolitoceanCommon::logger
                                PolitoceanCommon::MqttClient
                                PolitoceanCommon::mqttLogger
                                PolitoceanCommon::Sensor
                                PolitoceanCommon::Component
                                PolitoceanHmi::Serial
                                PolitoceanHmi::ipcamera
                                PolitoceanHmi::vision
                                PolitoceanHmi::autodrive
                                Qt5::Core
                                Qt5::Gui
                                Qt5::Widgets
                                ${OpenCV_LIBS}
                                -lpthread)

target_compile_features(mainwindow PRIVATE cxx_auto_type)
target_compile_options(mainwindow PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS mainwindow
        EXPORT PolitoceanHmiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
