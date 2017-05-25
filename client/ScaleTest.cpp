#include "ScaleTest.h"

using namespace std;

ScaleTest::ScaleTest() {
    _state = ENTER;
}

void ScaleTest::FSM(int dbSize) {
    string prompt;
    string scaleAlgorithmString;
    vector<string> options;
    int responseValue = 0;
    int transactionCount = 0;
    int initialThreadCount = 0;
    int finalThreadCount = 0;
    ScaleAlgorithm scaleAlgorithmType;

    while(true) {
        switch(_state) {
            case ENTER: {
                cout << "------------------------------" << endl;
                cout << "|        SCALE TESTING       |" << endl;
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
                    cout << "The total number of transactions must be at least 2. To execute a single transaction, use "
                         "Manual testing instead\n" << endl;
                    _state = EXIT;
                }
                else {
                    _state = INITIAL_THREAD_COUNT;
                }
                break;
            }
            case INITIAL_THREAD_COUNT: {
                prompt = "How many concurrent transaction threads should there be in the beginning?";
                responseValue = Utility::PromptUser(prompt, 1, transactionCount - 1);
                initialThreadCount = responseValue;

                _state = FINAL_THREAD_COUNT;
                break;
            }
            case FINAL_THREAD_COUNT: {
                prompt = "How many concurrent transaction threads should there be in the end?";
                responseValue = Utility::PromptUser(prompt, initialThreadCount + 1, transactionCount);
                finalThreadCount = responseValue;

                _state = SCALE_METHOD;
                break;
            }
            case SCALE_METHOD: {
                prompt = "Which method of scaling should be used to increase the concurrent thread count?";
                options = vector<string> {
                        "Linear",
                        "Quadratic",
                        "Exponential"
                };
                responseValue = Utility::PromptUser(prompt, options);

                switch(responseValue) {
                    case 1: {
                        scaleAlgorithmType = LINEAR;
                        scaleAlgorithmString = "Linear";
                        break;
                    }
                    case 2: {
                        scaleAlgorithmType = QUADRATIC;
                        scaleAlgorithmString = "Quadratic";
                        break;
                    }
                    case 3: {
                        scaleAlgorithmType = EXPONENTIAL;
                        scaleAlgorithmString = "Exponential";
                        break;
                    }
                    default: {
                        scaleAlgorithmString = "Linear";
                        scaleAlgorithmType = LINEAR;
                        break;
                    }
                }

                if (responseValue == 1) {
                    scaleAlgorithmType = LINEAR;
                }
                else if(responseValue == 2) {
                    scaleAlgorithmType = QUADRATIC;
                }
                else {
                    scaleAlgorithmType = EXPONENTIAL;
                }

                _state = BEGIN_TRANSACTION;
                break;
            }
            case BEGIN_TRANSACTION: {
                cout << "HeckaDBMS will now begin performing the transactions with the following parameters:" << endl;
                cout << "\t-Transactions: " << transactionCount << endl;
                cout << "\t-Initial Thread Count: " << initialThreadCount << endl;
                cout << "\t-Final Thread Count: " << finalThreadCount << endl;
                cout << "\t-Scaling Algorithm: " << scaleAlgorithmString << endl;
                cout << "Launching threads...\n" << endl;

                //TODO: Pass input to TM to launch transactions
                //Inputs: transactionCount, initialThreadCount, finalThreadCount, scaleAlgorithmType

                _state = END_TRANSACTION;
                break;
            }
            case END_TRANSACTION: {
                prompt = "What would you like to do next?";
                options = vector<string> {
                        "Specify another Scale test",
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