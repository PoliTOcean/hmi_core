
#include "JoystickPublisher.h"
#include "Joystick.h"
#include <iostream>

using namespace std;


int main(int argc, const char *argv[])
{
    JoystickPublisher publisher(JoystickPublisher::DFLT_ADDRESS, JoystickPublisher::DFLT_CLIENT_ID);

    try {
        Joystick joystick;

        cout << "Connecting to the publisher..." << endl;
        try{
            publisher.connect();
        }
        catch(exception e){
            cout << e.what() << endl;
            exit(EXIT_FAILURE);
        }

        joystick.startListening(&JoystickPublisher::callback, &publisher)->join();
        publisher.disconnect();
    } catch (Joystick::JoystickException& e) {
        cerr << e.what() << endl;
    } catch (mqtt::exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
