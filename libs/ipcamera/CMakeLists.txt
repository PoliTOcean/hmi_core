cmake_minimum_required(VERSION 3.5)
project(ipcamera VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(ipcamera
        ipcamera.cpp)

add_library(PolitoceanHmi::ipcamera ALIAS ipcamera)

target_include_directories(ipcamera
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ipcamera 
                                PolitoceanCommon::logger
                                ${OpenCV_LIBS}
                                -lflycapture)

target_compile_features(ipcamera PRIVATE cxx_auto_type)
target_compile_options(ipcamera PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS ipcamera
        EXPORT PolitoceanHmiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
