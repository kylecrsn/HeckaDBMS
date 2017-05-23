#include <iostream>

#include "Utility.h"

using namespace std;

int main(int argc, char *argv[])
{
    string protocolType;
    string testMetric;

    // Parse command-line arguments
    Utility::ParseArgs(argc, argv, &protocolType, &testMetric);

    // Check options
    cout << "options:" << endl << endl;
    cout << "protocol_type: " << protocolType << endl;
    cout << "test_metric: " << testMetric << endl;

    exit(0);
}