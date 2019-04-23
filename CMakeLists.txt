cmake_minimum_required(VERSION 3.5)
project(JoystickPublisher)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_subdirectory(politocean_common)

add_library(joystick
        src/Joystick.cpp)
target_link_libraries(joystick -lpthread)

add_executable(JoystickPublisher JoystickPublisher.cpp)
target_link_libraries(JoystickPublisher joystick Politocean::Publisher)
