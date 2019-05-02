//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H

#include <string>
#include <vector>

#include "json.hpp"
#include "Publisher.h"


namespace Politocean
{

class JoystickPublisher : public Publisher {
    unsigned char lastButton_;

public:
    static const std::string DFLT_ADDRESS, DFLT_CLIENT_ID, DFLT_TOPIC;
    static const std::string DFLT_TOPIC_AXIS, DFLT_TOPIC_BUTTON;

    JoystickPublisher() : JoystickPublisher(DFLT_ADDRESS, DFLT_CLIENT_ID) {}

    JoystickPublisher(const std::string& address, const std::string& clientID) : Publisher(address, clientID), lastButton_(0) {}

    void callback(const std::vector<int>& axes, unsigned char button);
};

}


#endif //JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H