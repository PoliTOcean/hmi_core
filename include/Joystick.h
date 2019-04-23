//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICK_H
#define JOYSTICKPUBLISHER_JOYSTICK_H

#include <linux/joystick.h>

#include <string>
#include <map>
#include <thread>
#include <exception>
#include <functional>

class Joystick
{
    int fd, num_of_axes, num_of_buttons;
    char name_of_joystick[80];
    struct js_event js;

    bool _isListening;

    /**
     * @axes and @buttons maps store respectively values for joystick axes and buttons.
     */
    std::map<int,int> axes, buttons;

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
     * @pFunction is a pointer to a (optional) function to execute within the listening thread.
     */
    template<class M, class T>
    std::thread* startListening(void (T::*fp)(std::map<int,int>axes, std::map<int,int> buttons), M *obj)
    {
        if (isListening())
            return nullptr;

        _isListening = true;

        return new std::thread([this, fp, obj]() {
            while (isListening())
            {
                readData();
                
                std::bind(fp, obj, std::placeholders::_1, std::placeholders::_2)(axes, buttons);
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

#endif //JOYSTICKPUBLISHER_JOYSTICK_H
