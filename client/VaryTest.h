#ifndef HECKADBMS_VARYTEST_H
#define HECKADBMS_VARYTEST_H

#include <string>
#include <iostream>
#include <vector>

#include "Utility.h"
#include "DataManager.h"
#include "TransactionManager.h"

using namespace std;

#define TRANSACTION_LIMIT 10000

class VaryTest {
public:
    /// Default constructor
    VaryTest();

    /// Finite state machine for processing user input
    void FSM(DataManager *dataManager, TransactionManager *transactionManager, bool isHekaton);

    /// State definition for a Vary testflow
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
