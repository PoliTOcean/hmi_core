//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H

#include <string>
#include <map>
#include "json.hpp"
#include "Publisher.h"


namespace Politocean {

class JoystickPublisher : public Publisher {
public:
    static const std::string DFLT_ADDRESS, DFLT_CLIENT_ID, DFLT_TOPIC;

    JoystickPublisher(const std::string& address, const std::string& clientID) : Publisher(address, clientID) {}

    void callback(std::map<int,int> axes, std::map<int,int>buttons);
};



}


#endif //JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H