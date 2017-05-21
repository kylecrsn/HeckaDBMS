#include <iostream>
#include "unistd.h"
#include <getopt.h>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
    int optIndex;
    int c;
    opterr = 0;
    string protocolType;
    string testMetric;
    string usage = "Usage: $ ./heckadbms -p (2pl|hekaton) [-t (manual|scale|vary)]";
    static struct option longOptions[] = {
            {"protocol_type",   required_argument,  0,  'p'},
            {"test_metric",     required_argument,  0,  't'},
            {0, 0, 0, 0}
        };

    // Parse command-line arguments
    while(1) {
        // Initialize the index and c
        optIndex = 0;
        c = getopt_long(argc, argv, "p:t:", longOptions, &optIndex);

        // Make sure the end hadn't been reached
        if(c == -1) {
            break;
        }

        // Cycle through the arguments
        switch(c) {
            case 'p': {
                protocolType = optarg;
                break;
            }
            case 't': {
                testMetric = optarg;
                break;
            }
            case '?': {
                cout << usage << endl;
                exit(1);
            }
            default: {
                cout << usage << endl;
                exit(1);
            }
        }
    }

    // Post-parsing error handling
    if(protocolType == "") {
        cout << usage << endl;
        exit(1);
    }
    transform(protocolType.begin(), protocolType.end(), protocolType.begin(), ::tolower);
    if(protocolType != "2pl" && protocolType != "hekaton") {
        cout << "Error: invalid protocol_type parameter: " << protocolType << endl;
        cout << usage << endl;
        exit(1);
    }

    if(testMetric == "") {
        testMetric = "manual";
    }
    else {
        transform(testMetric.begin(), testMetric.end(), testMetric.begin(), ::tolower);
        if(testMetric != "manual" && testMetric != "scale" && testMetric != "vary")
        {
            cout << "Error: invalid protocol_type parameter: " << testMetric << endl;
            cout << usage << endl;
            exit(1);
        }
    }

    // Check options
    cout << "options:" << endl << endl;
    cout << "protocol_type: " << protocolType << endl;
    cout << "test_metric: " << testMetric << endl;

    exit(0);
}