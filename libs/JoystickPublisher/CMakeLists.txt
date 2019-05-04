cmake_minimum_required(VERSION 3.5)
project(JoystickPublisher VERSION 1.0.0 LANGUAGES CXX)

add_library(JoystickPublisher
        JoystickPublisher.cpp)

add_library(PolitoceanHmi::JoystickPublisher ALIAS JoystickPublisher)

target_include_directories(JoystickPublisher
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(JoystickPublisher PolitoceanCommon::Publisher PolitoceanCommon::logger)

target_compile_features(JoystickPublisher PRIVATE cxx_auto_type)
target_compile_options(JoystickPublisher PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS JoystickPublisher
        EXPORT PolitoceanHmiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
