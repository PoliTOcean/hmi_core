#include "rovcontrol.h"

RovControl::RovControl()
{
    //* INIT GUI *//
    gui.show();

    client = std::make_shared<mqtt::async_client>(HOST_ROV,GUI_ID);

    connOpts = std::make_shared<mqtt::connect_options>();
    connOpts->set_keep_alive_interval(20);
    connOpts->set_clean_session(true);

    cb = new callback(*client,*connOpts);
    cb->setGui(&gui);
    client->set_callback(*cb);

    try {
        std::cout << "Connecting to the MQTT server..." << std::flush;
        client->connect(*connOpts, nullptr, *cb);
    }
    catch (const mqtt::exception&) {
        std::cerr << "\nERROR: Unable to connect to MQTT server: '" << std::endl;

    }


}
