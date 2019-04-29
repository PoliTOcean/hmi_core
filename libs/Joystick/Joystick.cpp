//
// Created by pettinz.
//

#include "Joystick.h"
#include "logger.h"
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <sys/ioctl.h>

namespace Politocean {

const std::string Joystick::DFLT_DEVICE { "/dev/input/js0" };

Joystick::Joystick()
    : Joystick(DFLT_DEVICE)
{
}

Joystick::Joystick(const std::string& device)
    : num_of_axes(0), num_of_buttons(0), _isListening(false)
{
    if ((fd = open(device.c_str(), O_RDONLY)) == -1) throw JoystickException("Joystick device not found.");

    ioctl(fd, JSIOCGAXES, &num_of_axes);
    ioctl(fd, JSIOCGBUTTONS, &num_of_buttons);
    ioctl(fd, JSIOCGNAME(80), name_of_joystick);

    std::stringstream info;
    info << "Joystick detected: " << name_of_joystick << "\n\t";
    info << num_of_axes << " axis\n\t";
    info << num_of_buttons << "buttons";
    logger::log(logger::DEBUG, info.str());

    fcntl(fd, F_SETFL, O_NONBLOCK);
}

Joystick::~Joystick()
{
    close(fd);
}

void Joystick::stopListening()
{
    if (!isListening())
        return ;

    _isListening = false;
}

void Joystick::readData()
{
    read(fd, &js, sizeof(struct js_event));

    switch (js.type & ~JS_EVENT_INIT)
    {
        case JS_EVENT_AXIS:
            axes[js.number] = js.value;
            break;
        case JS_EVENT_BUTTON:
            buttons[js.number] = js.value;
            break;
    }
}

int Joystick::getAxis(int axis)
{
    return axes[axis];
}

bool Joystick::getButton(int button)
{
    return (bool)buttons[button];
}

bool Joystick::isListening() { return _isListening; }

}