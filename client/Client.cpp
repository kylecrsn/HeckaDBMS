#include "Client.h"

using namespace std;

Client::Client() {
    _state = ENTER;
}

void Client::FSM(DataManager *dataManager, TransactionManager *transactionManager, bool isHekaton) {
    string prompt;
    string response;
    vector<string> options;
    int responseValue = 0;
    ManualTest manualTest;
    ScaleTest scaleTest;
    VaryTest varyTest;
    bool contextIsHekaton = isHekaton;

    while(true) {
        switch(_state) {
            case ENTER: {
                cout << "-------------------------------" << endl;
                cout << "|          MAIN MENU          |" << endl;
                cout << "-------------------------------" << endl;

                cout << "(Protocol: ";
                if(contextIsHekaton) {
                    cout << "Hekaton)";
                }
                else {
                    cout << "2PL)";
                }
                cout << endl;

                if(dataManager->getDb().size() == 0) {
                    cout << "(There is currently no in-memory database)\n" << endl;
                }
                else {
                    cout << "(In-memory database: " << dataManager->getDb().size() << " entries)\n" << endl;
                }

                _state = MAIN_MENU;
                break;
            }
            case MAIN_MENU: {
                prompt = "What would you like to do?";
                options = vector<string> {
                    "Choose a type of testing metric to execute",
                    "Switch protocol type",
                    "Generate an in-memory database",
                    "Load an existing database file into memory",
                    "Save the current in-memory database to disk",
                    "Clear the current in-memory database",
                    "Print the in-memory database to the console",
                    "Exit"
                };

                responseValue = Utility::PromptUser(prompt, options);
                if(responseValue == 1) {
                    if(dataManager->getDb().size() == 0) {
                        cout << "(!) There is no in-memory database. First either generate one or load an existing one\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = TEST_METRICS;
                    }
                }
                else if(responseValue == 2) {
                    _state = PROTOCOL;
                }
                else if(responseValue == 3) {
                    if(dataManager->getDb().size() == 0) {
                        _state = GENERATE_DATA;
                    }
                    else {
                        cout << "(!) There is an existing in-memory database. First either clear it or save it to disk\n" << endl;
                        _state = ENTER;
                    }
                }
                else if(responseValue == 4) {
                    if(dataManager->getDb().size() == 0) {
                        _state = LOAD_DATA;
                    }
                    else {
                        cout << "(!) There is an existing in-memory database. First either clear it or save it to disk\n" << endl;
                        _state = ENTER;
                    }
                }
                else if(responseValue == 5) {
                    if(dataManager->getDb().size() == 0) {
                        cout << "(!) There is no in-memory database to save\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = SAVE_DATA;
                    }
                }
                else if(responseValue == 6) {
                    if(dataManager->getDb().size() == 0) {
                        cout << "(!) There is no in-memory database to clear\n" << endl;
                        _state = ENTER;
                    }
                    else {
                        _state = CLEAR_DATA;
                    }
                }
                else if(responseValue == 7) {
                    if(dataManager->getDb().size() == 0) {
                        cout << "(!) There is no in-memory database to print\n" << endl;
                        _state = ENTER;
                    }
                    else if(dataManager->getDb().size() > DATA_PRINT_LIMIT) {
                        cout << "(!) Printing databases larger than " << DATA_PRINT_LIMIT << " is disallowed\n" << endl;
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
                    "Vary (Simulate a static-sized transaction load with a varied ratio of read-only to read-write "
                        "transactions)"
                };

                responseValue = Utility::PromptUser(prompt, options);
                if(responseValue == 1) {
                    manualTest = ManualTest();
                    manualTest.FSM(dataManager, transactionManager, contextIsHekaton);
                }
                else if(responseValue == 2) {
                    scaleTest = ScaleTest();
                    scaleTest.FSM(dataManager, transactionManager, contextIsHekaton);
                }
                else {
                    varyTest = VaryTest();
                    varyTest.FSM(dataManager, transactionManager, contextIsHekaton);
                }

                _state = ENTER;
                break;
            }
            case PROTOCOL: {
                cout << "You have switched the protocol from ";
                if(contextIsHekaton) {
                    cout << "Hekaton to 2PL\n";
                }
                else {
                    cout << "2PL to Hekaton\n";
                }
                cout << endl;
                contextIsHekaton = !contextIsHekaton;

                _state = ENTER;
                break;
            }
            case GENERATE_DATA: {
                prompt = "How many entries should the database initially be?";

                responseValue = Utility::PromptUser(prompt, 1, 10000000);
                dataManager->generateDatabase(responseValue);
                cout << "A database with " << responseValue << " entries was successfully generated\n" << endl;

				dataManager->setSize(responseValue);
				
                _state = ENTER;
                break;
            }
            case LOAD_DATA: {
                fstream filestream;
                prompt = "What is the database filename to load? (Example: database.csv)";

                response = Utility::PromptUser(prompt);
                filestream.open(response, fstream::in);
                if(filestream.fail()) {
                    cout << "(!) The given filename does not exist or could not be found/accessed\n" << endl;
                }
                else {
                    filestream.close();
                    dataManager->loadDatabase(response);
                }

                _state = ENTER;
                break;
            }
            case SAVE_DATA: {
                struct stat buf;
                prompt = "What should the database file be named? (Example: database.csv)";

                response = Utility::PromptUser(prompt);
                if (stat(response.c_str(), &buf) != -1) {
                    prompt = "That file already exists. Would you like to overwrite it?";
                    options = {
                        "Yes",
                        "No"
                    };
                    responseValue = Utility::PromptUser(prompt, options);

                    if(responseValue == 1) {
                        dataManager->saveDatabase(response);
                        cout << "The file was successfully overwritten\n" << endl;
                    }
                    else {
                        cout << "The file was not overwritten\n" << endl;
                    }
                }
                else {
                    dataManager->saveDatabase(response);
                    cout << "The file was successfully saved to disk\n" << endl;
                }

                _state = ENTER;
                break;
            }
            case CLEAR_DATA: {
                dataManager->clearDatabase();
                cout << "The current in-memory database has been cleared\n" << endl;

                _state = ENTER;
                break;
            }
            //TODO: Readable or storage formats
            case PRINT_DATA: {
                prompt = "Which format should be used to print the database?";
                options = {
                    "Compact (Same format data is stored on disk)",
                    "Verbose (Much more readable/user-friendly)"
                };

                responseValue = Utility::PromptUser(prompt, options);
                cout << "Below is the contents of the current in-memory database" << endl;
                cout << "(NOTE: The data may be printed in any order as the internal data structures used do not specify "
                        "an explicit ordering)\n";
                cout << "(NOTE: The NextRecord pointer is specified by the EntryKey of the record it points to)\n" << endl;
                if(responseValue == 1) {
                    dataManager->printDatabaseCompact();
                }
                else {
                    dataManager->printDatabaseVerbose();
                }
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