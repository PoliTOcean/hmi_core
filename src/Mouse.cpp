#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>

#include <json.hpp>

#include "MqttClient.h"

#include "PolitoceanConstants.h"
#include "PolitoceanUtils.hpp"

#include <X11/Xlib.h>

#define X_MOUSE 9
#define Y_MOUSE 10

using namespace Politocean;

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
    return !axes_.empty();
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

    while(1)
    {
        if (!listener.isAxesUpdated())
            continue ;

        std::vector<int> contr = listener.axes();
        contr[X_MOUSE] = Politocean::map(contr[X_MOUSE], SHRT_MIN, SHRT_MAX, 0, width);
        contr[Y_MOUSE] = Politocean::map(contr[Y_MOUSE], SHRT_MIN, SHRT_MAX, 0, height);

        XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, contr[X_MOUSE], contr[Y_MOUSE]);
        XFlush(dpy);

        std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}

