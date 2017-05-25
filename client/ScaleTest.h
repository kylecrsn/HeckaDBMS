#ifndef HECKADBMS_SCALETEST_H
#define HECKADBMS_SCALETEST_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Utility.h"

using namespace std;

#define TRANSACTION_LIMIT 10000
#define PRINT_KEY_LIMIT 10

class ScaleTest {
public:
    // Default constructor
    ScaleTest();

    // Finite state machine for processing user input
    void FSM(int dbSize);

    // State definition for a scale testflow
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

    // Types of scaling methods
    enum ScaleAlgorithm {
        LINEAR,
        QUADRATIC,
        EXPONENTIAL
    };

private:
    ScaleState _state;
};

#endif
