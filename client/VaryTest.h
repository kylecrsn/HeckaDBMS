#ifndef HECKADBMS_VARYTEST_H
#define HECKADBMS_VARYTEST_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Utility.h"

using namespace std;

#define TRANSACTION_LIMIT 10000

class VaryTest {
public:
    // Default constructor
    VaryTest();

    // Finite state machine for processing user input
    void FSM(int dbSize);

    // State definition for a Vary testflow
    enum VaryState {
        ENTER,
        TRANSACTIONS,
        THREAD_COUNT,
        RO_PERCENTAGE,
        BEGIN_TRANSACTION,
        END_TRANSACTION,
        EXIT
    };

private:
    VaryState _state;
};

#endif
