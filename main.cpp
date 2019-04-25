
#include "JoystickPublisher.h"
#include "Joystick.h"
#include <iostream>
#include "PolitoceanExceptions.hpp"

using namespace std;
using namespace Politocean;

int main(int argc, const char *argv[])
{
    JoystickPublisher publisher(JoystickPublisher::DFLT_ADDRESS, JoystickPublisher::DFLT_CLIENT_ID);

    try {
        Joystick joystick;

        cout << "Connecting to the publisher..." << endl;
        try{
            publisher.connect();
        }
        catch(Politocean::mqttException& e){
            cout << e.what() << endl;
            exit(EXIT_FAILURE);
        }

        joystick.startListening(&JoystickPublisher::callback, &publisher)->join();
        publisher.disconnect();
    } catch (Joystick::JoystickException& e) {
        cerr << e.what() << endl;
    } catch (Politocean::mqttException& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
