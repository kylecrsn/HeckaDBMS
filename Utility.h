#ifndef HECKADBMS_UTILITY_H
#define HECKADBMS_UTILITY_H

#include <iostream>
#include <getopt.h>
#include <algorithm>

#include "Operation.h"
#include "DataManager.h"

using namespace std;

class Utility {
public:
    /// Prompt the use for a string response of their choice
    static string PromptUser(string prompt);

    /// Prompt the user for an integer response within the range defined by min and max inclusively
    static int PromptUser(string prompt, int min, int max);

    /// Prompt the user for an integer response corresponding to a specific option
    static int PromptUser(string prompt, const vector<string>& options);

    /// Generate a vector of random read-only operations
    static vector<Operation> getRandomReadOnlyOps(DataManager *dataManager, int opCount);

    /// Generate a vector of random read-write operations, in the order read, write, read, write
    static vector<Operation> getRandomReadWriteOps(DataManager *dataManager, int opCount);

private:
    /// Default constructor (static class, no implementation)
    Utility() {}
};

#endif
