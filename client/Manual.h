#ifndef HECKADBMS_MANUAL_H
#define HECKADBMS_MANUAL_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Utility.h"

using namespace std;

class Manual {
public:
    // Default constructor
    Manual();

    // Finite state machine for processing user input
    void FSM(int dbSize);

    // State definition for a manual testflow
    enum ClientState {
        ENTER,
        RO_TRANSACTIONS,
        RW_TRANSACTIONS,
        THREAD_COUNT,
        DATA_SELECT,
        DATA_SPECIFY,
        BEGIN_TRANSACTION,
        END_TRANSACTION,
        EXIT };

private:
    ClientState _state;
};

#endif
