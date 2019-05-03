cmake_minimum_required(VERSION 3.5)
project(autodrive VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(autodrive
        autodrive.cpp)

add_library(PolitoceanHmi::autodrive ALIAS autodrive)

target_include_directories(autodrive
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(autodrive 
                                PolitoceanCommon::logger
                                PolitoceanCommon::Publisher
                                ${OpenCV_LIBS}
                                -lflycapture
                                PolitoceanHmi::vision)

target_compile_features(autodrive PRIVATE cxx_auto_type)
target_compile_options(autodrive PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS autodrive
        EXPORT PolitoceanHmiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
