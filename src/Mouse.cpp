#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>

#include <unistd.h>

#include <json.hpp>

#include "MqttClient.h"

#include "PolitoceanConstants.h"
#include "PolitoceanUtils.hpp"

#include <X11/Xlib.h>

#define X 0
#define Y 1
#define PX_MAX_STEP     50
#define AXIS_OFFSET     4681

using namespace Politocean;
using namespace Politocean::Constants;

typedef struct button_t
{
    int id;
    unsigned int value;

    button_t(int id, int value) : id(id), value(value) {}

    bool operator==(const button_t &o) const {
        return id == o.id && value == o.value;
    }

} button_t;

class Listener {
    std::queue<button_t> buttons_;

    bool isButtonUpdated_ = false;
    bool isAxesUpdated_ = false;

    std::vector<int> axes_;

public:
    void listenForButtons(const std::string& payload);
    void listenForAxes(const std::string& payload);

    button_t button();

    std::vector<int> axes();

    bool isButtonUpdated();
    bool isAxesUpdated();
};

void Listener::listenForAxes(const std::string& payload)
{
    auto c_map = nlohmann::json::parse(payload);
    axes_ = c_map.get<std::vector<int>>();
	
    isAxesUpdated_ = true;
}

void Listener::listenForButtons(const std::string& payload)
{
    int btn = static_cast<int>(std::stoi(payload));

    buttons_.push(button_t(btn & 0x7F, (btn >> 7) & 0x01));

    isButtonUpdated_ = true;
}

button_t Listener::button()
{
    if (!buttons_.empty())
    {
        button_t button = buttons_.front();
        buttons_.pop();
        return button;
    }
    return button_t(-1, 0);
}

bool Listener::isButtonUpdated()
{
    return !buttons_.empty();
}

bool Listener::isAxesUpdated()
{
    return isAxesUpdated_ && !axes_.empty();
}

std::vector<int> Listener::axes(){
    isAxesUpdated_ = false;
    return axes_;
}

int main(void) {
    
    MqttClient& subscriber = MqttClient::getInstance("hmi", Hmi::IP_ADDRESS);
    
    Listener listener;

    subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    subscriber.subscribeTo(Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);

    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    int height = DisplayHeight(dpy, scr);
    int width  = DisplayWidth(dpy, scr);
    std::cout << "Screen size : x: " << width << "\thegiht: " << height << std::endl;

    bool click = false;
    std::vector<int> lastMouse(2, 0);
    while(1)
    {
        if (!listener.isAxesUpdated())
            continue;

        if (listener.isButtonUpdated())
        {
            button_t button = listener.button();
            if (button.value && button.id == Commands::Buttons::CLICK_MOUSE)
                click = true;
        }
        
        std::vector<int> axes = listener.axes();
        std::vector<int> newMouse(2, 0);
        int dx = Politocean::map(axes[Commands::Axes::X_MOUSE]-AXIS_OFFSET, SHRT_MIN, SHRT_MAX, -PX_MAX_STEP, PX_MAX_STEP);
        int dy = Politocean::map(axes[Commands::Axes::Y_MOUSE]-AXIS_OFFSET, SHRT_MIN, SHRT_MAX, -PX_MAX_STEP, PX_MAX_STEP);
        
        int bini;
        unsigned int binui;
        Window binw;
        XQueryPointer(dpy, root_window, &binw, &binw, &lastMouse[X], &lastMouse[Y], &bini, &bini, &binui);

        newMouse[X] = lastMouse[X]+dx;
        if(newMouse[X] < 0) newMouse[X] = 0;
        else if(newMouse[X] > width) newMouse[X] = width;

        newMouse[Y] = lastMouse[Y]+dy; 
        if(newMouse[Y] < 0) newMouse[Y] = 0;
        else if(newMouse[Y] > height) newMouse[Y] = height;
        
        XWarpPointer(dpy, None, root_window, lastMouse[X], lastMouse[Y], 0, 0, newMouse[X], newMouse[Y]);
        lastMouse = newMouse;

        if (click) {
            XEvent event;
            memset(&event, 0x00, sizeof(event));
            event.type = ButtonPress;
            event.xbutton.button = Button1;
            event.xbutton.same_screen = True;
            event.xbutton.root = root_window;
            event.xbutton.window = root_window;
            event.xbutton.subwindow = 0;
            event.xbutton.x_root = lastMouse[X];
            event.xbutton.y_root = lastMouse[Y];
            event.xbutton.x = lastMouse[X];
            event.xbutton.y = lastMouse[Y];
            event.xbutton.state = 0;

            XSendEvent(dpy, root_window, True, ButtonPressMask, &event);
            XFlush(dpy);
            usleep(100000);

            event.type = ButtonRelease;
            event.xbutton.state = 0x100;
            XSendEvent(dpy, root_window, True, ButtonReleaseMask, &event);
            click = false;
        }

        XFlush(dpy);
	}
}

