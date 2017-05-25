#include "Utility.h"

using namespace std;

void Utility::ParseArgs(int argc, char* argv[], string* protocolType) {
    int optIndex;
    int c;
    opterr = 0;
    string usage = "Usage: $ ./heckadbms -p (2pl|hekaton)";
    static struct option longOptions[] = {
            {"protocol_type",   required_argument,  0,  'p'},
            {0, 0, 0, 0}
    };

    // Parse command-line arguments
    while(1) {
        // Initialize the index and c
        optIndex = 0;
        c = getopt_long(argc, argv, "p:", longOptions, &optIndex);

        // Make sure the end hadn't been reached
        if(c == -1) {
            break;
        }

        // Cycle through the arguments
        switch(c) {
            case 'p': {
                *protocolType = optarg;
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
    if(*protocolType == "") {
        cout << usage << endl;
        exit(1);
    }
    transform((*protocolType).begin(), (*protocolType).end(), (*protocolType).begin(), ::tolower);
    if(*protocolType != "2pl" && *protocolType != "hekaton") {
        cout << "Error: invalid protocol_type parameter: " << protocolType << "\n" << usage << endl;
        exit(1);
    }
}

int Utility::PromptUser(string prompt, int min, int max) {
    string response;
    int responseValue;

    PROMPT:
    cout << prompt << " [" << min << ", " << max << "]\n\n> " << flush;
    getline(cin, response);
    cout << endl;

    try {
        responseValue = stoi(response);
    }
    catch (invalid_argument e) {
        cout << "ERROR: User gave an invalid response:\n\t" << e.what() << "\nPlease try again.\n" << endl;
        goto PROMPT;
    }
    if(responseValue < min || responseValue > max) {
        cout << "Please enter a valid integer within the specified range\n" << endl;
        goto PROMPT;
    }

    return responseValue;
}

int Utility::PromptUser(string prompt, const vector<string>& options) {
    string response;
    int responseValue;

    PROMPT:
    cout << prompt << endl;
    for (vector<string>::const_iterator it = options.begin(); it != options.end(); it++) {
        cout << "[" << it - options.begin() + 1 << "] " << *it << "\n";
    }
    cout << "\n> " << flush;
    getline(cin, response);
    cout << endl;

    try {
        responseValue = stoi(response);
    }
    catch (invalid_argument e) {
        cout << "ERROR: User gave an invalid response:\n\t" << e.what() << "\nPlease try again.\n" << endl;
        goto PROMPT;
    }
    if(responseValue < 1 || responseValue > options.size()) {
        cout << "Please enter one of the valid values inside the brackets\n" << endl;
        goto PROMPT;
    }

    return responseValue;
}

string Utility::PromptUser(string prompt) {
    string response;

    cout << prompt << "\n\n> " << flush;
    getline(cin, response);
    cout << endl;

    return response;
}