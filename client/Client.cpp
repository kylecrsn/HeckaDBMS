#include "Client.h"

using namespace std;

Client::Client() {
    _state = ENTER;
}

void Client::FSM() {
    string prompt;
    vector<string> options;
    int responseValue = 0;
    bool saveDatabase = false;
    ManualTest manualTest;
    ScaleTest scaleTest;
    VaryTest varyTest;

    while(true) {
        switch(_state) {
            case ENTER: {
                _state = MAIN_MENU;
                break;
            }
            case MAIN_MENU: {
                prompt = "What would you like to do?";
                options = vector<string> {
                    "Choose a type of testing metric to execute",
                    "Exit and save the current database state",
                    "Exit and don't save the current database state"
                };
                responseValue = Utility::PromptUser(prompt, options);

                if(responseValue == 1) {
                    _state = TEST_METRIC;
                }
                else if(responseValue == 2) {
                    saveDatabase = true;
                    _state = EXIT;
                }
                else {
                    saveDatabase = false;
                    _state = EXIT;
                }
                break;
            }
            case TEST_METRIC: {
                prompt = "Which testing metric would you like to execute?";
                options = vector<string> {
                    "Manual (Specify explicit details to launch a batch of transaction attempts)",
                    "Scale (Simulate an increasingly scaling concurrent transaction load with an even ratio of "
                        "read-only and read-write transactions",
                    "Vary (Simulate a static transaction load with a varied ratio of read-only to read-write "
                        "transactions)"
                };
                responseValue = Utility::PromptUser(prompt, options);

                if(responseValue == 1) {
                    manualTest = ManualTest();
                    manualTest.FSM(2);
                }
                else if(responseValue == 2) {
                    scaleTest = ScaleTest();
                    //TODO: scaleTest.FSM();
                }
                else {
                    varyTest = VaryTest();
                    //TODO: varyTest.FSM();
                }

                _state = ENTER;
                break;
            }
            case EXIT: {
                if(saveDatabase) {
                    //TODO
                }
                else {
                    //TODO
                }

                return;
            }
            default: {
                _state = EXIT;
                break;
            }
        }
    }
}