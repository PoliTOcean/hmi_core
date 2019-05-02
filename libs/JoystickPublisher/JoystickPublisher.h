//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H

#include <string>
#include <vector>
#include <thread>

#include "json.hpp"
#include "Publisher.h"


namespace Politocean
{

class JoystickPublisher : public Publisher {
    unsigned char button_;
    std::vector<int> axes_;

    bool isPublishingAxes_, isPublishingButtons_, isPublishing_;

    std::thread *publishAxes_, *publishButtons_;
    std::thread *publishAxes(), *publishButtons();

public:
    static const std::string DFLT_ADDRESS, DFLT_CLIENT_ID, DFLT_TOPIC;
    static const std::string DFLT_TOPIC_AXIS, DFLT_TOPIC_BUTTON;

    JoystickPublisher() : JoystickPublisher(DFLT_ADDRESS, DFLT_CLIENT_ID) {}

    JoystickPublisher(const std::string& address, const std::string& clientID)
        : Publisher(address, clientID), isPublishingAxes_(false), isPublishingButtons_(false), isPublishing_(false) {}

    void callback(const std::vector<int>& axes, unsigned char button);

    void startPublishing();
    void stopPublishing();

    bool isPublishing();
};

}


#endif //JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H