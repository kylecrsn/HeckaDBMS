#include "ManualTest.h"

using namespace std;

ManualTest::ManualTest() {
    _state = ENTER;
}

void ManualTest::FSM(int dbSize) {
    string prompt;
    stringstream ss;
    vector<string> options;
    int responseValue = 0;
    int readOnlyCount = 0;
    int readWriteCount = 0;
    int threadCount = 0;
    vector<int> readOnlyKeys;
    vector<int> readWriteKeys;
    bool specifyDataObjects = false;

    while(true) {
        switch(_state) {
            case ENTER: {
                cout << "------------------------------" << endl;
                cout << "|       MANUAL TESTING       |" << endl;
                cout << "------------------------------\n" << endl;

                _state = RO_TRANSACTIONS;
                break;
            }
            case RO_TRANSACTIONS: {
                prompt = "How many Read-Only transactions do you want to perform?";
                readOnlyCount = Utility::PromptUser(prompt, 0, RO_TRANSACTION_LIMIT);

                _state = RW_TRANSACTIONS;
                break;
            }
            case RW_TRANSACTIONS: {
                prompt = "How many Read-Write transactions do you want to perform?";
                readWriteCount = Utility::PromptUser(prompt, 0, RW_TRANSACTION_LIMIT);

                if(readOnlyCount + readWriteCount == 0) {
                    cout << "The total number of transactions must be at least 1\n" << endl;
                    _state = EXIT;
                }
                else {
                    _state = THREAD_COUNT;
                }
                break;
            }
            case THREAD_COUNT: {
                prompt = "How many concurrent threads would you like attempting these transactions?";
                threadCount = Utility::PromptUser(prompt, 1, readOnlyCount + readWriteCount);

                _state = DATA_SELECT;
                break;
            }
            case DATA_SELECT: {
                prompt = "Do you want to specify each data object to perform each transaction on, or randomly select them "
                        "from the database?";
                options = vector<string> {
                    "Randomly select them from the database",
                    "Specify each data object (NOTE: if there are lots of transactions this may be tedious)"
                };
                responseValue = Utility::PromptUser(prompt, options);

                if (responseValue == 1) {
                    specifyDataObjects = false;
                } else {
                    specifyDataObjects = true;
                }

                _state = DATA_SPECIFY;
                break;
            }
            case DATA_SPECIFY: {
                mt19937 gen;
                gen.seed(random_device()());
                uniform_real_distribution<> distribution(0, dbSize - 1);
                readOnlyKeys.clear();
                readWriteKeys.clear();

                if (specifyDataObjects) {
                    for (int i = 0; i < readOnlyCount; i++) {
                        ss << "Which data object key would you like to perform read-only transaction " << i << " on?";
                        prompt = ss.str();
                        readOnlyKeys.push_back(Utility::PromptUser(prompt, 0, dbSize - 1));
                    }
                    for (int i = 0; i < readWriteCount; i++) {
                        ss << "Which data object key would you like to perform read-write transaction " << i << " on?";
                        prompt = ss.str();
                        readWriteKeys.push_back(Utility::PromptUser(prompt, 0, dbSize - 1));
                    }
                }
                else {
                    for(int i = 0; i < readOnlyCount; i++) {
                        readOnlyKeys.push_back((int)distribution(gen));
                    }
                    for(int i = 0; i < readWriteCount; i++) {
                        readWriteKeys.push_back((int)distribution(gen));
                    }
                }

                _state = BEGIN_TRANSACTION;
                break;
            }
            case BEGIN_TRANSACTION: {
                cout << "HeckaDBMS will now begin performing the transaction(s) with the following parameters:" << endl;
                cout << "\t-Read-Only Transactions: " << readOnlyCount << endl;
                cout << "\t-Read-Only Data Object Keys: ";
                for(int i = 0; i < min(readOnlyCount, PRINT_KEY_LIMIT); i++) {
                    if(i == PRINT_KEY_LIMIT - 1 && readOnlyCount != PRINT_KEY_LIMIT) {
                        cout << readOnlyKeys.at(i) << "...";
                    }
                    else if(i == readOnlyCount - 1) {
                        cout << readOnlyKeys.at(i);
                    }
                    else {
                        cout << readOnlyKeys.at(i) << ", ";
                    }
                }
                cout << endl;
                cout << "\t-Read-Write Transactions: " << readWriteCount << endl;
                cout << "\t-Read-Write Data Object Keys: ";
                for(int i = 0; i < min(readWriteCount, PRINT_KEY_LIMIT); i++) {
                    if(i == PRINT_KEY_LIMIT - 1 && readWriteCount != PRINT_KEY_LIMIT) {
                        cout << readWriteKeys.at(i) << "...";
                    }
                    else if(i == readWriteCount - 1) {
                        cout << readWriteKeys.at(i);
                    }
                    else {
                        cout << readWriteKeys.at(i) << ", ";
                    }
                }
                cout << endl;
                cout << "\t-Concurrent Threads: " << threadCount << endl;
                cout << "Launching threads...\n" << endl;

                //TODO: Pass input to TM to launch transactions
                //Inputs: readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys, threadCount

                _state = END_TRANSACTION;
                break;
            }
            case END_TRANSACTION: {
                prompt = "What would you like to do next?";
                options = vector<string> {
                    "Specify another Manual test",
                    "Return to the main menu"
                };

                cout << "The transaction(s) have completed with the following metric information:\n" << endl;

                //TODO: include metric info about transaction results

                responseValue = Utility::PromptUser(prompt, options);

                if(responseValue == 1) {
                    _state = ENTER;
                }
                else {
                    _state = EXIT;
                }
                break;
            }
            case EXIT: {
                return;
            }
            default: {
                _state = EXIT;
                break;
            }
        }
    }
}