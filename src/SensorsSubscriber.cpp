#include <thread>
#include <chrono>

#include "Subscriber.h"
#include "Sensor.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"

#include "json.hpp"

/********************************************************
 * Listener class for sensors
 *******************************************************/

using namespace Politocean;

class Listener
{
    std::vector<int> sensors_;

    bool sensorsUpdated_;

public:
    Listener() : sensorsUpdated_(false) {}

    void listen(const std::string& payload);

    std::vector<int> sensors();

    bool isSensorsUpdated();
};

void Listener::listen(const std::string& payload)
{
    auto c_map = nlohmann::json::parse(payload);
	sensors_ = c_map.get<std::vector<int>>();
	
	sensorsUpdated_ = true;
}

std::vector<int> Listener::sensors()
{
    return sensors_;
}

bool Listener::isSensorsUpdated()
{
    return sensorsUpdated_;
}

/********************************************************
 * Main section
 *******************************************************/

int main(int argc, const char *argv[])
{
    logger::enableLevel(logger::DEBUG, true);
    Subscriber subscriber(Constants::Hmi::IP_ADDRESS, Constants::Hmi::SENSORS_ID_SUB);
    Listener listener;

    subscriber.subscribeTo(Constants::Topics::SENSORS, &Listener::listen, &listener);

    // Try to connect subscriber to sensors topic
    try
    {
        subscriber.connect();
    }
    catch(const mqttException::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    while (subscriber.is_connected())
        if (listener.isSensorsUpdated())
        {
            std::vector<int> sensors = listener.sensors();

            for (auto sensor_type : Politocean::sensor_t())
            {
                switch (sensor_type)
                {
                    case sensor_t::PITCH:
                        std::cout << "\tPITCH: ";
                        break;
                    
                    case sensor_t::PRESSION:
                        std::cout << "\tPRESSURE: ";
                        break;
                    
                    case sensor_t::ROLL:
                        std::cout << "ROLL: ";
                        break;
                    
                    case sensor_t::TEMPERATURE:
                        std::cout << "\tTEMPERATURE: ";
                        break;
                    
                    default: break;
                }

			    std::cout << sensors[static_cast<int>(sensor_type)];
            }

            std::cout << std::endl;
        }

    return 0;
}