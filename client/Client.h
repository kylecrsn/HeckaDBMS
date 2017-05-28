#ifndef HECKADBMS_CLIENT_H
#define HECKADBMS_CLIENT_H

#include <sys/stat.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "DataManager.h"
#include "Utility.h"
#include "ManualTest.h"
#include "ScaleTest.h"
#include "VaryTest.h"

using namespace std;

#define DATA_PRINT_LIMIT 20

class Client {
public:
    /// Default constructor
    Client();

    /// Finite state machine for processing user input
    void FSM(DataManager dataManager);

    /// State definition for the top-level client menu
    enum ClientState {
        ENTER,
        MAIN_MENU,
        TEST_METRICS,
        GENERATE_DATA,
        LOAD_DATA,
        SAVE_DATA,
        CLEAR_DATA,
        PRINT_DATA,
        EXIT
    };

private:
    ClientState _state;
};

#endif