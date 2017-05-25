#include "Client.h"

using namespace std;

Client::Client() {
    _state = ENTER;
}

void Client::FSM(DataManager dataManager, string protocolType) {
    string prompt;
    string response;
    vector<string> options;
    int responseValue = 0;
    ManualTest manualTest;
    ScaleTest scaleTest;
    VaryTest varyTest;

    while(true) {
        switch(_state) {
            case ENTER: {
                cout << "-------------------------------" << endl;
                cout << "|          MAIN MENU          |" << endl;
                cout << "-------------------------------" << endl;

                if(dataManager.GetDB().size() == 0) {
                    cout << "(There is currently no in-memory database)\n" << endl;
                }
                else {
                    cout << "(In-memory database: " << dataManager.GetDB().size() << " entries)\n" << endl;
                }

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
                    "Clear the current in-memory database",
                    "Print the in-memory database to the console",
                    "Exit"
                };
                responseValue = Utility::PromptUser(prompt, options);

                if(responseValue == 1) {
                    if(dataManager.GetDB().size() == 0) {
                        cout << "There is no in-memory database to use. First either generate one or load an existing one\n" << endl;
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
                        cout << "There is an existing in-memory database. First either clear it or save it to disk\n" << endl;
                        _state = ENTER;
                    }
                }
                else if(responseValue == 3) {
                    if(dataManager.GetDB().size() == 0) {
                        _state = LOAD_DATA;
                    }
                    else {
                        cout << "There is an existing in-memory database. First either clear it or save it to disk\n" << endl;
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
                else if(responseValue == 5) {
                    if(dataManager.GetDB().size() == 0) {
                        cout << "There is no in-memory database to clear\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = CLEAR_DATA;
                    }
                }
                else if(responseValue == 6) {
                    if(dataManager.GetDB().size() == 0) {
                        cout << "There is no in-memory database to print\n" << endl;
                        _state = ENTER;
                    }
                    else if(dataManager.GetDB().size() > DATA_PRINT_LIMIT) {
                        cout << "Printing databases larger than " << DATA_PRINT_LIMIT << " is disallowed\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = PRINT_DATA;
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
                    manualTest.FSM(dataManager.GetDB().size());
                }
                else if(responseValue == 2) {
                    scaleTest = ScaleTest();
                    scaleTest.FSM(dataManager.GetDB().size());
                }
                else {
                    varyTest = VaryTest();
                    varyTest.FSM(dataManager.GetDB().size());
                }

                _state = ENTER;
                break;
            }
            case GENERATE_DATA: {
                prompt = "How many entries should the database initially be?";
                responseValue = Utility::PromptUser(prompt, 1, 10000000);

                dataManager.GenerateDataSet(responseValue);

                cout << "A database with " << responseValue << " entries was successfully generated\n" << endl;

                _state = ENTER;
                break;
            }
            case LOAD_DATA: {
                fstream filestream;
                prompt = "What is the database filename to load? (Example: database.csv)";
                response = Utility::PromptUser(prompt);

                filestream.open(response, fstream::in);
                if(filestream.fail()) {
                    cout << "The given filename does not exist or could not be found/accessed\n" << endl;
                }
                else {
                    filestream.close();
                    if(dataManager.LoadDataSet(response, protocolType)) {
                        cout << "The specified file was successfully loaded into the memory database\n" << endl;
                    }
                    else {
                        cout << "The specified file was meant for use with a different transaction protocol\n" << endl;
                    }
                }

                _state = ENTER;
                break;
            }
            case SAVE_DATA: {
                struct stat buf;
                prompt = "What should the database file be named?";
                response = Utility::PromptUser(prompt);

                if (stat(response.c_str(), &buf) != -1) {
                    prompt = "That file already exists. Would you like to overwrite it?";
                    options = {
                        "Yes",
                        "No"
                    };
                    responseValue = Utility::PromptUser(prompt, options);

                    if(responseValue == 1) {
                        dataManager.SaveDataSet(response, protocolType);
                        cout << "The file was successfully overwritten\n" << endl;
                    }
                    else {
                        cout << "The file was not overwritten\n" << endl;
                    }
                }
                else {
                    dataManager.SaveDataSet(response, protocolType);
                    cout << "The file was successfully saved to disk\n" << endl;
                }

                _state = ENTER;
                break;
            }
            case CLEAR_DATA: {
                dataManager.ClearDataSet();
                cout << "The current in-memory database has been cleared\n" << endl;

                _state = ENTER;
                break;
            }
            case PRINT_DATA: {
                cout << "Below is the contents of the current in-memory database" << endl;
                cout << "NOTE: The data may be printed in any order as the internal data structures used do not specify "
                    "an explicit ordering\n" << endl;
                dataManager.PrintDataSet();
                cout << endl;

                _state = ENTER;
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