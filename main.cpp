
#include "JoystickPublisher.h"
#include "Joystick.h"
#include <iostream>
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

#define MAX_TRIES 10

using namespace std;
using namespace Politocean;

int main(int argc, const char *argv[])
{
    JoystickPublisher publisher(JoystickPublisher::DFLT_ADDRESS, JoystickPublisher::DFLT_CLIENT_ID);
    bool connected = false;

    logger::enableLevel(logger::DEBUG, true);

    try {
        Joystick joystick;

        cout << "Connecting to the publisher..." << endl;
        int n_tries = 0;
        while (!connected && n_tries < MAX_TRIES){
            ++n_tries;
            cout << "Attempt " << n_tries << " to connect..." << endl;
            try{
                publisher.connect();
                connected = true;
            }
            catch(Politocean::mqttException& e){
                cout << "MQTT error: " << e.what() << endl;
                connected = false;
            }
        }

        joystick.startListening(&JoystickPublisher::callback, &publisher)->join();
        publisher.disconnect();
    } catch (std::exception& e) {
        cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
