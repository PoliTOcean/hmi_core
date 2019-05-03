#include <iostream>
#include <unistd.h>

#include "Joystick.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "JoystickPublisher.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

#define MAX_TRIES 10

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

Publisher pub("127.0.0.1", "Joystick_client");
mqttLogger ptoLogger(&pub);

void testcb(const std::string& payload){
    cout << payload << endl;
}
Subscriber sub("127.0.0.1", "testhmi");

int main(int argc, const char *argv[])
{
    sub.connect();
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

    sub.wait();

    return 0;
}
