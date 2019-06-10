#include <iostream>

#include "Serial.hpp"

int main(int argc, const char *argv[])
{
    Serial serial("/dev/ttyACM0");
    serial.open();

    std::string str = "", old = "";
    while (1)
    {
        serial.readLine(str);

        if (str == old)
            continue ;
        
        std::cout << str;
        old = str;
    }

    serial.close();
    
    return 0;
}