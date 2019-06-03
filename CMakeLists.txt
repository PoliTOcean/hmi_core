cmake_minimum_required(VERSION 3.5)
project(PolitoceanHmi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
	In-source builds are not allowed.
	Please, create a separate directory for build files (e.g. build/).
	Delete CMakeCache.txt, make a separate dir and try again.
	")
endif()

# Add politocean common libs
add_subdirectory(politocean_common)
include_directories(politocean_common/include)

# Add vision libs
#add_subdirectory(vision)

# Add project internal libs
add_subdirectory(libs)

# Add executable
add_executable(JoystickPublisher src/JoystickPublisher.cpp)
add_executable(Gui src/gui.cpp)
add_executable(CommandParser src/CommandParser.cpp)

#test section
find_package(Catch2 REQUIRED)
add_executable(MqttTest test/MqttTest.cpp)
target_link_libraries(MqttTest Catch2::Catch2
    PolitoceanCommon::MqttClient
    PolitoceanCommon::mqttLogger)
    
# Linking the libraries
target_link_libraries(JoystickPublisher -lpthread
    PolitoceanHmi::Joystick
    PolitoceanCommon::mqttLogger
    PolitoceanCommon::MqttClient)

target_link_libraries(Gui -lpthread
    # Add here all the libraries as:
    # Name_of_the_library       --- if it's internal (e.g. Publisher)
    # -lname_of_the_library     --- if it's a standard c++ library (e.g. -lpthread)
    PolitoceanHmi::mainwindow
    PolitoceanCommon::mqttLogger
    PolitoceanCommon::MqttClient
    PolitoceanCommon::logger)
    
target_link_libraries(CommandParser -lpthread
        PolitoceanCommon::mqttLogger
        PolitoceanCommon::MqttClient)

