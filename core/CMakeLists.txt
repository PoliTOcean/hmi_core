cmake_minimum_required(VERSION 3.13)
project(JoystickPublisher)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_library(joystick
        src/Joystick.cpp)
target_link_libraries(joystick -lpthread)

add_executable(JoystickPublisher main.cpp src/JoystickPublisher.cpp include/JoystickPublisher.h)
target_link_libraries(JoystickPublisher joystick -lpaho-mqttpp3)