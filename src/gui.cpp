#include <iostream>
#include <unistd.h>

#include "Publisher.h"
#include "Subscriber.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

Publisher pub("127.0.0.1", Hmi::CLIENT_ID);
mqttLogger ptoLogger(&pub);

void testcb(const std::string& payload){
    cout << payload << endl;
}
Subscriber sub("127.0.0.1", "testhmi");

int main(int argc, const char *argv[])
{
    sub.connect();
    logger::enableLevel(logger::DEBUG, true);

    bool connected = false;
    sub.wait();

    return 0;
}
