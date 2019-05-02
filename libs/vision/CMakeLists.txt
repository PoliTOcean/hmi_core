cmake_minimum_required(VERSION 3.5)
project(vision VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(vision
        vision.cpp)

add_library(PolitoceanHmi::vision ALIAS vision)

target_include_directories(vision
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(vision 
                                PolitoceanCommon::logger
                                ${OpenCV_LIBS})

target_compile_features(vision PRIVATE cxx_auto_type)
target_compile_options(vision PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS vision
        EXPORT PolitoceanHmiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
