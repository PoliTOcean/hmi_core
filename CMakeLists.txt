cmake_minimum_required(VERSION 3.5)
project(PolitoceanHmi)

set(CMAKE_CXX_STANDARD 11)

# Add politocean common libs
add_subdirectory(politocean_common)
include_directories(politocean_common/include)

# Add vision libs
#add_subdirectory(vision)

# Add project internal libs
add_subdirectory(libs)

# Add executable
add_executable(MainJoystick joystick.cpp)
add_executable(Gui gui.cpp)

# Linking the libraries
target_link_libraries(MainJoystick
    # Add here all the libraries as:
    # Name_of_the_library       --- if it's internal (e.g. Publisher)
    # -lname_of_the_library     --- if it's a standard c++ library (e.g. -lpthread)
    PolitoceanHmi::Joystick 
    PolitoceanHmi::JoystickPublisher
    PolitoceanCommon::mqttLogger
    PolitoceanCommon::Publisher
    PolitoceanCommon::Subscriber)

target_link_libraries(Gui 
    # Add here all the libraries as:
    # Name_of_the_library       --- if it's internal (e.g. Publisher)
    # -lname_of_the_library     --- if it's a standard c++ library (e.g. -lpthread)
    PolitoceanCommon::mqttLogger
    PolitoceanCommon::Publisher
    PolitoceanCommon::Subscriber)
