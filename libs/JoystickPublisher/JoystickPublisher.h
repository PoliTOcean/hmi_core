//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H

#include <string>
#include <vector>

#include "json.hpp"
#include "Publisher.h"


namespace Politocean {

class JoystickPublisher : public Publisher {
public:
    static const std::string DFLT_ADDRESS, DFLT_CLIENT_ID, DFLT_TOPIC;

    JoystickPublisher() : JoystickPublisher(DFLT_ADDRESS, DFLT_CLIENT_ID) {}

    JoystickPublisher(const std::string& address, const std::string& clientID) : Publisher(address, clientID) {}

    void callback(std::vector<int>& axes, std::vector<int>& buttons);
};



}


#endif //JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H