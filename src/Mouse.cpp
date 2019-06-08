#include <X11/Xlib.h>
#include <iostream>
#include </usr/include/unistd.h>
#include <json.hpp>
#include "MqttClient.h"
#include "PolitoceanUtils.hpp"


class Listener {
    std::queue<button_t> buttons_;

    bool isButtonUpdated_ = false;
    bool isAxesUpdated_ = false;

    std::vector<int> axes_;

    std::mutex mutexAxs_, mutexBtn_;


public:
    void listenForButtons(const std::string& payload);
    void listenForAxes(const std::string& payload);

    button_t button();

    std::vector<int> axes();

    bool isButtonUpdated();
    bool isAxesUpdated();
};

void Listener::listenForButtons(const std::string& payload)
{
	std::lock_guard<std::mutex> lock(mutexBtn_);
    int btn  = static_cast<int>(std::stoi(payload));

    buttons_.push( button_t(btn & 0x7F, (btn >> 7) & 0x01) );

    isButtonUpdated_ = true;
}

button_t Listener::button()
{
   	std::lock_guard<std::mutex> lock(mutexBtn_);
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

void Listener::listenForAxes(const std::string& payload)
{
   	std::lock_guard<std::mutex> lock(mutexAxs_);
    auto c_map = nlohmann::json::parse(payload);
    std::vector<int> tmp = c_map.get<std::vector<int>>();
	axes_ = {tmp[9], tmp[10]};
	
    isAxesUpdated_ = true;
}

std::vector<int> Listener::axes(){
   	std::lock_guard<std::mutex> lock(mutexAxs_);
    isAxesUpdated_ = false;
    return axes_;
}
int main(void) {
	
	MqttClient& subscriber = MqttClient::getInstance(Rov::ATMEGA_ID, Rov::IP_ADDRESS);
	
	Listener listener;

	// Subscribe @subscriber to joystick publisher topics 9 10 bott 30
	subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    subscriber.subscribeTo(Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);



    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    int height = DisplayHeight(dpy, scr);
    int width  = DisplayWidth(dpy, scr);
	std::vector<int> contr;
	
	while(1){
		contr = listener.axes();
		contr[0] = Politocean::map(contr[0], SHRT_MIN, SHRT_MAX, 0, widht);
		contr[1] = Politocean::map(contr[1], SHRT_MIN, SHRT_MAX, 0, height);
		std::cout << "Screen size : " << width << "x" << height << std::endl;


		XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, contr[0], contr[1]);
		XFlush(dpy);
		usleep(50);
	}
}

