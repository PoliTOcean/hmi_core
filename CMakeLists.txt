cmake_minimum_required(VERSION 3.5)
project(PolitoceanHmi)

set(CMAKE_CXX_STANDARD 11)

# Add politocean common libs
add_subdirectory(politocean_common)
add_compile_definitions(WEBCAM_MODE)
include_directories(politocean_common/include)

# Add vision libs
#add_subdirectory(vision)

# Add project internal libs
add_subdirectory(libs)

# Add executable
add_executable(JoystickPublisher src/JoystickPublisher.cpp)
add_executable(Gui src/gui.cpp)
add_executable(CommandParser src/CommandParser.cpp)
add_executable(SensorsSubscriber src/SensorsSubscriber.cpp)

# Linking the libraries
target_link_libraries(JoystickPublisher -lpthread
    PolitoceanHmi::Joystick

    PolitoceanCommon::mqttLogger
    PolitoceanCommon::Publisher
    PolitoceanCommon::Subscriber)

target_link_libraries(Gui 
    # Add here all the libraries as:
    # Name_of_the_library       --- if it's internal (e.g. Publisher)
    # -lname_of_the_library     --- if it's a standard c++ library (e.g. -lpthread)
    PolitoceanHmi::mainwindow

    PolitoceanCommon::Subscriber
    PolitoceanCommon::logger)
    
target_link_libraries(CommandParser -lpthread
        PolitoceanCommon::mqttLogger
        PolitoceanCommon::Publisher
        PolitoceanCommon::Subscriber)

target_link_libraries(SensorsSubscriber -lpthread
        PolitoceanCommon::mqttLogger
        PolitoceanCommon::Publisher
        PolitoceanCommon::Subscriber
        PolitoceanCommon::Sensor)
