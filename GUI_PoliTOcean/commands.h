//
// Created by pettinz on 31/03/19.
//

#ifndef TRYSPI_COMMANDS_H
#define TRYSPI_COMMANDS_H

namespace COMMANDS {
    namespace BUTTONS {
        static const int    MOTORS_ON   = 1,
                            MOTORS_OFF  = 2,
                            VDOWN       = 5,
                            WRIST       = 7,
                            RESET       = 9,
                            VUP         = 14,
                            MEDIUM_FAST = 24,
                            SLOW        = 25;
    }

    namespace AXIS {
        static const int    X           = 0,
                            Y           = 1,
                            SHOULDER    = 2,
                            WRIST       = 4,
                            RZ          = 5;
    }
}


#endif //TRYSPI_COMMANDS_H
