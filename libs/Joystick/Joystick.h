/**
 * @author: pettinz
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <linux/joystick.h>

#include <string>
#include <vector>
#include <thread>
#include <exception>
#include <functional>

namespace Politocean {

class Joystick
{
    int fd, num_of_axes, num_of_buttons;
    char name_of_joystick[80];
    struct js_event js;

    bool _isListening;

    /**
     * @axes and @buttons maps store respectively values for joystick axes and buttons.
     */
    std::vector<int> axes;
    unsigned char button;

public:
    static const std::string DFLT_DEVICE;
    /**
     * Opens the joystick file descriptor @fd.
     * It throws a @JoystickException if the open fails.
     */

    Joystick();
    explicit Joystick(const std::string& device);
    /**
     * Closes the joystick file descriptor @fd.
     */
    ~Joystick();

    /*
     * Reads data from joystick and stores them.
     */
    void readData();
    /**
     * Returns a thread which is listening to the joystick.
     * @fp is a pointer to the method function
     * @obj is the pointer to the instance object
     */
    template<class M, class T>
    std::thread* startListening(void (T::*fp)(std::vector<int>& axes, unsigned char button), M *obj)
    {
        if (isListening())
            return nullptr;

        _isListening = true;

        auto callbackFunction = std::bind(fp, obj, std::placeholders::_1, std::placeholders::_2);

        return new std::thread([this, callbackFunction]() {
            while (isListening())
            {
                readData();
                
                callbackFunction(axes, button);
            }
        });
    }
    /**
     * It stops the listening thread by setting @_isListening to false.
     */
    void stopListening();

    /**
     * Returns the value respectively for the @axis axis and @button button inside the maps.
     */
    int getAxis(int axis);
    bool getButton(int button);

    /*
     * Return true if there is a thread listening to the joystick, false otherwise.
     */
    bool isListening();

    /*
     * A class that inherits from std::exception, to handle joystick exceptions
     */
    class JoystickException : public std::exception {
        std::string msg;

    public:
        explicit JoystickException(const std::string &msg) : msg(msg) {}

        const char* what() const noexcept override {
            return msg.c_str();
        }
    };
};

}


#endif //JOYSTICK_H
