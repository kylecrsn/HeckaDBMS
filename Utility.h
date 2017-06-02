#ifndef HECKADBMS_UTILITY_H
#define HECKADBMS_UTILITY_H

#include <iostream>
#include <getopt.h>
#include <algorithm>

using namespace std;

class Utility {
public:
    /// Parse command-line arguments
    static void ParseArgs(int argc, char* argv[], string* protocolType);

    /// Prompt the use for a string response of their choice
    static string PromptUser(string prompt);

    /// Prompt the user for an integer response within the range defined by min and max inclusively
    static int PromptUser(string prompt, int min, int max);

    /// Prompt the user for an integer response corresponding to a specific option
    static int PromptUser(string prompt, const vector<string>& options);

    /// Types of scaling methods
    enum ScaleAlgorithm {
        LINEAR,
        QUADRATIC,
        EXPONENTIAL
    };

private:
    /// Default constructor (static class, no implementation)
    Utility() {}
};

#endif
