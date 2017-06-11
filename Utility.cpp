#include "Utility.h"

using namespace std;

string Utility::PromptUser(string prompt) {
    string response;

    cout << prompt << "\n\n> " << flush;
    getline(cin, response);
    cout << endl;

    return response;
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
        cout << "(!) Please enter a valid integer within the specified range\n" << endl;
        goto PROMPT;
    }
    if(responseValue < min || responseValue > max) {
        cout << "(!) Please enter a valid integer within the specified range\n" << endl;
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
        cout << "(!) Please enter one of the valid values inside the brackets\n" << endl;
        goto PROMPT;
    }
    if(responseValue < 1 || responseValue > options.size()) {
        cout << "(!) Please enter one of the valid values inside the brackets\n" << endl;
        goto PROMPT;
    }

    return responseValue;
}

vector<Operation> Utility::getRandomReadOnlyOps(DataManager *dataManager, int opCount) {
    vector<Operation> ops;
    Record *record;
    mt19937 gen;
    gen.seed(random_device()());
//    uniform_real_distribution<> keysDist(0, dataManager->getDb().size() - 1);
    uniform_real_distribution<> keysDist(0, dataManager->getSize() - 1);
    for(int i = 0; i < opCount; i++) {
        ops.push_back(Operation());
        ops.back().setMode(Operation::READ);
        record = NULL;
        while(record == NULL) {
            record = dataManager->getDb()[(int)keysDist(gen)];
        }
        while(!record->getIsLatest()) {
            record = record->getNextRecord();
        }
        ops.back().setKey(record->getEntryKey());
    }
    return ops;
}

vector<Operation> Utility::getRandomReadWriteOps(DataManager *dataManager, int opCount) {
    vector<Operation> ops;
    Record *record;
    mt19937 gen;
    gen.seed(random_device()());
//    uniform_real_distribution<> keysDist(0, dataManager->getDb().size() - 1);
    uniform_real_distribution<> keysDist(0, dataManager->getSize() - 1);
    uniform_real_distribution<> valuesDist(0, 1000);

    for(int i = 0; i < opCount; i++) {
        ops.push_back(Operation());
        if(i % 2 == 0) {
            ops[i].setMode(Operation::READ);
        }
        else {
        	ops[i].setMode(Operation::WRITE);
            ops[i].setValue((int)valuesDist(gen));
        }

        record = NULL;
        while(record == NULL) {
            record = dataManager->getDb()[(int)keysDist(gen)];
        }
        while(!record->getIsLatest()) {
            record = record->getNextRecord();
        }
        ops[i].setKey(record->getEntryKey());
    }

    return ops;
}
