#include "VaryTest.h"

using namespace std;

VaryTest::VaryTest() {
    _state = ENTER;
}

void VaryTest::FSM(DataManager *dataManager, TransactionManager *transactionManager) {
    string prompt;
    vector<string> options;
    int responseValue = 0;
    int transactionCount = 0;
    int threadCount = 0;
    int roPercentage = 0;

    while(true) {
        switch(_state) {
            case ENTER: {
                cout << "------------------------------" << endl;
                cout << "|        VARY TESTING        |" << endl;
                cout << "------------------------------\n" << endl;

                _state = TRANSACTIONS;
                break;
            }
            case TRANSACTIONS: {
                prompt = "How many transactions do you want to perform?\n(NOTE: Specifying 0 or 1 will send you back to "
                        "the Main Menu)";
                responseValue = Utility::PromptUser(prompt, 0, TRANSACTION_LIMIT);

                transactionCount = responseValue;
                if(transactionCount < 2) {
                    cout << "(!) The total number of transactions must be at least 2. To execute a single transaction, use "
                            "Manual testing instead\n" << endl;
                    _state = EXIT;
                }
                else {
                    _state = THREAD_COUNT;
                }
                break;
            }
            case THREAD_COUNT: {
                prompt = "How many concurrent transaction threads should there be?";
                responseValue = Utility::PromptUser(prompt, 1, transactionCount);
                threadCount = responseValue;

                _state = RO_PERCENTAGE;
                break;
            }
            case RO_PERCENTAGE: {
                prompt = "What percentage of the transactions should be read-only?";
                options = {
                    "0%",
                    "1%",
                    "20%",
                    "40%",
                    "50%",
                    "60%",
                    "80%",
                    "99%",
                    "100%"
                };
                responseValue = Utility::PromptUser(prompt, options);

                switch(responseValue) {
                    case 1: {
                        roPercentage = 0;
                        break;
                    }
                    case 2: {
                        roPercentage = 1;
                        break;
                    }
                    case 3: {
                        roPercentage = 20;
                        break;
                    }
                    case 4: {
                        roPercentage = 40;
                        break;
                    }
                    case 5: {
                        roPercentage = 50;
                        break;
                    }
                    case 6: {
                        roPercentage = 60;
                        break;
                    }
                    case 7: {
                        roPercentage = 80;
                        break;
                    }
                    case 8: {
                        roPercentage = 99;
                        break;
                    }
                    case 9: {
                        roPercentage = 100;
                        break;
                    }
                    default: {
                        roPercentage = 0;
                        break;
                    }
                }

                _state = BEGIN_TRANSACTION;
                break;
            }
            case BEGIN_TRANSACTION: {
                cout << "HeckaDBMS will now begin performing the transactions with the following parameters:" << endl;
                cout << "\t|Transactions: " << transactionCount << endl;
                cout << "\t|Thread Count: " << threadCount << endl;
                cout << "\t|Read-Only Transaction Percentage: " << roPercentage << "%" << endl;
                cout << "Launching threads...\n" << endl;

                //TODO: Pass input to TM to launch transactions
                //Inputs: transactionCount, threadCount, roPercentage

                _state = END_TRANSACTION;
                break;
            }
            case END_TRANSACTION: {
                prompt = "What would you like to do next?";
                options = vector<string> {
                        "Specify another Vary test",
                        "Return to the Main Menu"
                };

                cout << "The transactions have completed with the following metric information:\n" << endl;

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