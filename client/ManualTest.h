#ifndef HECKADBMS_MANUALTEST_H
#define HECKADBMS_MANUALTEST_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "TransactionManager.h"
#include "Utility.h"
#include "DataManager.h"
#include "TransactionManager.h"

using namespace std;

#define RO_TRANSACTION_LIMIT 5000
#define RW_TRANSACTION_LIMIT 5000
#define PRINT_KEY_LIMIT 16

class ManualTest {
public:
    /// Default constructor
    ManualTest();

    /// Finite state machine for processing user input
    void FSM(DataManager *dataManager, TransactionManager *transactionManager);

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
