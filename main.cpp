
#include "JoystickPublisher.h"
#include "Publisher.h"
#include "Joystick.h"
#include <iostream>
#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"
#include <unistd.h>

#define MAX_TRIES 10

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

Publisher pub("127.0.0.1", Hmi::CLIENT_ID);
mqttLogger ptoLogger(&pub);

int main(int argc, const char *argv[])
{
    logger::enableLevel(logger::DEBUG, true);

    JoystickPublisher joystickPub;
    bool connected = false;

    try {
        Joystick joystick;

        ptoLogger.logInfo("Connecting to the publisher...");
        int n_tries = 0;
        while (!connected && n_tries < MAX_TRIES){
            ++n_tries;
            cout << "Attempt " << n_tries << " to connect..." << endl;
            try{
                joystickPub.connect();
                connected = true;
            }
            catch(Politocean::mqttException& e){
                cout << "MQTT error: " << e.what() << endl;
                connected = false;
            }
        }

        joystick.startListening(&JoystickPublisher::callback, &joystickPub)->join();
        joystickPub.disconnect();
    } catch (std::exception& e) {
        cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
