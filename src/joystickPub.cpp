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


void testcb(const std::string& payload){
    cout << payload << endl;
}

int main(int argc, const char *argv[])
{
    Publisher pub("169.254.98.217", "clientID");
    pub.connect();
    mqttLogger ptoLogger(&pub);
 //   logger::enableLevel(logger::DEBUG, true);

    JoystickPublisher joystickPub("169.254.98.217", "joystickPublisher");
    bool connected = false;

    try {
        Joystick joystick;

        ptoLogger.logInfo("Connecting to the publisher...");
        int n_tries = 0;
        while (!connected && n_tries < MAX_TRIES){
            ++n_tries;
            ptoLogger.logInfo(string("Attempt ") + to_string(n_tries) + string(" to connect..."));
            try{
                joystickPub.connect();
                connected = true;
            }
            catch(Politocean::mqttException& e){
                logger::log(logger::ERROR, e);
                connected = false;
            }
        }

        joystickPub.startPublishing();
        joystick.startListening(&JoystickPublisher::updateValues, &joystickPub)->join();
        joystickPub.disconnect();
    } catch (std::exception& e) {
        logger::log(logger::ERROR, e);
        exit(EXIT_FAILURE);
    }

    return 0;
}
