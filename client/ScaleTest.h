#ifndef HECKADBMS_SCALETEST_H
#define HECKADBMS_SCALETEST_H

#include <string>
#include <iostream>
#include <vector>

#include "Utility.h"
#include "DataManager.h"
#include "TransactionManager.h"

using namespace std;

#define TRANSACTION_LIMIT 10000

class ScaleTest {
public:
    /// Default constructor
    ScaleTest();

    /// Finite state machine for processing user input
    void FSM(DataManager *dataManager, TransactionManager *transactionManager);

    /// State definition for a Scale testflow
    enum ScaleState {
        ENTER,
        TRANSACTIONS,
        INITIAL_THREAD_COUNT,
        FINAL_THREAD_COUNT,
        SCALE_METHOD,
        BEGIN_TRANSACTION,
        END_TRANSACTION,
        EXIT
    };

private:
    ScaleState _state;
};

#endif
