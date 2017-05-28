#ifndef HECKADBMS_MANUALTEST_H
#define HECKADBMS_MANUALTEST_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "Utility.h"

using namespace std;

#define RO_TRANSACTION_LIMIT 100
#define RW_TRANSACTION_LIMIT 100
#define PRINT_KEY_LIMIT 10

class ManualTest {
public:
    /// Default constructor
    ManualTest();

    /// Finite state machine for processing user input
    void FSM(int dbSize);

    /// State definition for a Manual testflow
    enum ManualState {
        ENTER,
        RO_TRANSACTIONS,
        RW_TRANSACTIONS,
        THREAD_COUNT,
        DATA_SELECT,
        DATA_SPECIFY,
        BEGIN_TRANSACTION,
        END_TRANSACTION,
        EXIT
    };

private:
    ManualState _state;
};

#endif
