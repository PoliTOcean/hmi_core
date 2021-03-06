#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>

#include <unistd.h>

#include <json.hpp>

#include <mqttLogger.h>
#include <MqttClient.h>

#include "PolitoceanConstants.h"
#include "PolitoceanUtils.hpp"

#include "Button.hpp"
#include "Reflectables/Vector.hpp"

#include <X11/Xlib.h>

#define X 0
#define Y 1
#define PX_MAX_STEP     50
#define AXIS_OFFSET     4681

using namespace Politocean;
using namespace Politocean::Constants;

class Listener {
    std::queue<Button> buttons_;

    bool isButtonUpdated_ = false;
    bool isAxesUpdated_ = false;

    Types::Vector<int> axes_;

public:
    void listenForButtons(Button button);
    void listenForAxes(Types::Vector<int> axes);

    Button button();

    Types::Vector<int> axes();

    bool isButtonUpdated();
    bool isAxesUpdated();
};

void Listener::listenForAxes(Types::Vector<int> axes)
{
    axes_ = axes;
    isAxesUpdated_ = true;
}

void Listener::listenForButtons(Button button)
{
    buttons_.push(button);
    isButtonUpdated_ = true;
}

Button Listener::button()
{
    if (buttons_.empty())
        return Button(-1,0);

    Button button = buttons_.front();
    buttons_.pop();
    
    return button;
}

bool Listener::isButtonUpdated()
{
    return !buttons_.empty();
}

bool Listener::isAxesUpdated()
{
    return isAxesUpdated_ && !axes_.empty();
}

Types::Vector<int> Listener::axes(){
    isAxesUpdated_ = false;
    return axes_;
}

int main(int argc, char *argv[])
{
    mqttLogger::setRootTag(argv[0]);

    MqttClient& subscriber = MqttClient::getInstance(Hmi::MOUSE_ID, Hmi::IP_ADDRESS);
    
    Listener listener;

    subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    subscriber.subscribeTo(Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);

    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    int height = DisplayHeight(dpy, scr);
    int width  = DisplayWidth(dpy, scr);
    mqttLogger::getInstance().log(logger::CONFIG, string("Screen width: ")+to_string(width)+"\theight: "+to_string(height));

    bool click = false;
    while(1)
    {
        if (!listener.isAxesUpdated())
            continue;

        if (listener.isButtonUpdated())
        {
            Button button = listener.button();
            if (button.getValue() && button.getId() == Commands::Buttons::CLICK_MOUSE)
                click = true;
        }
        
        Types::Vector<int> axes = listener.axes(), lastMouse(2, 0), newMouse(2, 0);
        int dx = Politocean::map(axes[Commands::Axes::X_MOUSE]-AXIS_OFFSET, SHRT_MIN, SHRT_MAX, -PX_MAX_STEP, PX_MAX_STEP);
        int dy = Politocean::map(axes[Commands::Axes::Y_MOUSE]-AXIS_OFFSET, SHRT_MIN, SHRT_MAX, -PX_MAX_STEP, PX_MAX_STEP);
        
        XEvent event;
        memset (&event, 0, sizeof (event));
        event.xbutton.same_screen = True;
        event.xbutton.subwindow = DefaultRootWindow (dpy);
        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;
            XQueryPointer (dpy, event.xbutton.window,
                    &event.xbutton.root, &event.xbutton.subwindow,
                    &event.xbutton.x_root, &event.xbutton.y_root,
                    &event.xbutton.x, &event.xbutton.y,
                    &event.xbutton.state);
        }

        if (click) {
            event.type = ButtonPress;
            event.xbutton.button = Button1; // left button
            if (XSendEvent(dpy, PointerWindow, True, ButtonPressMask, &event)==0)
                mqttLogger::getInstance().log(logger::WARNING, "Error while sending the mouse button event.");
            XFlush(dpy);
            usleep(1000);

            event.type = ButtonRelease;
            if(XSendEvent(dpy, PointerWindow, True, ButtonReleaseMask, &event)==0)
                mqttLogger::getInstance().log(logger::WARNING, "Error while sending the mouse button event.");
            XFlush(dpy);
            click = false;
        }

        if (dx==0 && dy==0) continue;
        
        lastMouse[X] = event.xbutton.x_root;
        lastMouse[Y] = event.xbutton.y_root;
        newMouse[X] = lastMouse[X]+dx;
        if(newMouse[X] < 0) newMouse[X] = 0;
        else if(newMouse[X] > width) newMouse[X] = width;

        newMouse[Y] = lastMouse[Y]+dy; 
        if(newMouse[Y] < 0) newMouse[Y] = 0;
        else if(newMouse[Y] > height) newMouse[Y] = height;
        
        XWarpPointer(dpy, None, root_window, lastMouse[X], lastMouse[Y], 0, 0, newMouse[X], newMouse[Y]);
        lastMouse = newMouse;

        XFlush(dpy);
	}
}

