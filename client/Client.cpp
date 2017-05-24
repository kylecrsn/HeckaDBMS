#include "Client.h"

using namespace std;

Client::Client() {
    _state = ENTER;
}

void Client::FSM(DataManager dataManager) {
    string prompt;
    string response;
    vector<string> options;
    int responseValue = 0;
    int dbSize = 0;
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
                    "Generate an in-memory database",
                    "Load an existing database file into memory",
                    "Save the current in-memory database to disk",
                    "Exit"
                };
                responseValue = Utility::PromptUser(prompt, options);

                if(responseValue == 1) {
                    if(dataManager.GetDB().size() == 0) {
                        cout << "There is no in-memory database to use. Either generate one or load an existing one\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = TEST_METRICS;
                    }
                }
                else if(responseValue == 2) {
                    if(dataManager.GetDB().size() == 0) {
                        _state = GENERATE_DATA;
                    }
                    else {
                        cout << "There is an existing in-memory database. Either clear it or save it to disk\n" << endl;
                        _state = ENTER;
                    }
                }
                else if(responseValue == 3) {
                    if(dataManager.GetDB().size() == 0) {
                        _state = LOAD_DATA;
                    }
                    else {
                        cout << "There is an existing in-memory database. Either clear it or save it to disk\n" << endl;
                        _state = ENTER;
                    }
                }
                else if(responseValue == 4) {
                    if(dataManager.GetDB().size() == 0) {
                        cout << "There is no in-memory database to save\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = SAVE_DATA;
                    }
                }
                else {
                    _state = EXIT;
                }
                break;
            }
            case TEST_METRICS: {
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
                    manualTest.FSM(dbSize);
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
            case GENERATE_DATA: {
                prompt = "How many entries should the database initially be?";
                responseValue = Utility::PromptUser(prompt, 1, 10000000);

                dbSize = responseValue;
                dataManager.GenerateDataSet(dbSize);

                _state = ENTER;
                break;
            }
            case LOAD_DATA: {
                struct stat buffer;
                fstream filestream;
                prompt = "What is the database filename to load? (Example: database.csv)";
                response = Utility::PromptUser(prompt);

                if(stat(response.c_str(), &buffer) == 0) {
                    //TODO: Finish file loading
                }
                else {
                    cout << "Could not find the specified file\n" << endl;
                }

                _state = ENTER;
                break;
            }
            case SAVE_DATA: {
                prompt = "What should the database file be named?";
                response = Utility::PromptUser(prompt);
                //TODO: Finish file saving
                _state = ENTER;
                break;
            }
            case EXIT: {
                //TODO

                return;
            }
            default: {
                _state = EXIT;
                break;
            }
        }
    }
}