/**
 * @author pettinz
 */

#ifndef JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_H

#include <string>
#include <vector>
#include <thread>

#include "json.hpp"
#include "Publisher.h"


namespace Politocean
{

class JoystickPublisher : public Publisher {
    /**
     * @button_ stores the last button pressed or released on the joystick.
     * 
     * It is 8 bit unsigned integer with the following structure:
     *  (*) 1 bit (the MSB) represents the value (0 if released, 1 if pressed)
     *  (*) the remaining 7 bit represent the button identifier
     */
    unsigned char button_;
    /**
     * @axes_ stores the last value for each joystick axis
     *
     * The vector is structured as follows:
     *  (*) indices represents the axes identifiers
     *  (*) values represents the axes last values
     */
    std::vector<int> axes_;

    bool isPublishingAxes_, isPublishingButtons_, isPublishing_;

    std::thread *publishAxes_, *publishButtons_;
    /**
     * @publishAxes() returns a new thread that publishes every 50ns @axes_ vector on @DFLT_TOPIC_AXES
     * @publishButtons() returns a new thread that publishes @buttons_ value on @DFLT_TOPIC_BUTTONS
     */
    std::thread *publishAxes(), *publishButtons();

public:
    /**
     * @DFLT_ADDRESS    : default mosquitto address
     * @DFLT_CLIENT_ID  : default publisher client id
     * @DFLT_TOPIC      : default topic for publishing
     */
    static const std::string DFLT_ADDRESS, DFLT_CLIENT_ID, DFLT_TOPIC;
    /**
     * @DFLT_TOPIC_AXES     : default topic for publishing axes
     * @DFLT_TOPIC_BUTTONS  : default topic for publishing buttons
     */
    static const std::string DFLT_TOPIC_AXIS, DFLT_TOPIC_BUTTON;

    JoystickPublisher() : JoystickPublisher(DFLT_ADDRESS, DFLT_CLIENT_ID) {}

    JoystickPublisher(const std::string& address, const std::string& clientID)
        : Publisher(address, clientID), isPublishingAxes_(false), isPublishingButtons_(false), isPublishing_(false) {}

    /**
     * Reads @axes and @button values from joystick and stores them in the @axes_ and @buttons_ variables.
     */
    void updateValues(const std::vector<int>& axes, unsigned char button);

    /**
     * Starts publishing axes values on @DFLT_TOPIC_AXES every 50ms and starts publishing buttons values
     * on @DFLT_TOPIC_BUTTONS
     */
    void startPublishing();
    // It stops publishing values
    void stopPublishing();

    // Returns true if the publisher is publishing something to topics.
    bool isPublishing();
};

}


#endif //JOYSTICKPUBLISHER_H