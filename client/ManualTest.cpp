#include "ManualTest.h"

using namespace std;

ManualTest::ManualTest() {
    _state = ENTER;
}

void ManualTest::FSM(DataManager *dataManager, TransactionManager *transactionManager, bool isHekaton) {
    string prompt;
    stringstream ss;
    vector<string> options;
    int responseValue = 0;
    int readOnlyCount = 0;
    int readWriteCount = 0;
    int threadCount = 0;
    vector<Operation> readOnlyOps;
    vector<Operation> readWriteOps;
    int opPerTxn = dataManager->getOpsPerTransaction();
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
                prompt = "How many Read-Only transactions do you want to perform?\n"
                    "(NOTE: Each Read-Only transaction executes 4 Read operations)";
                readOnlyCount = Utility::PromptUser(prompt, 0, RO_TRANSACTION_LIMIT);

                _state = RW_TRANSACTIONS;
                break;
            }
            case RW_TRANSACTIONS: {
                prompt = "How many Read-Write transactions do you want to perform?\n"
                    "(NOTE: Each Read-Write transaction executes 2 Read and 2 Write operations)";
                readWriteCount = Utility::PromptUser(prompt, 0, RW_TRANSACTION_LIMIT);

                if(readOnlyCount + readWriteCount == 0) {
                    cout << "(!) The total number of transactions must be at least 1\n" << endl;
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
                prompt = "Do you want to randomly select objects from the database or specify each object to perform each"
                    " operation of each transaction on?";
                options = vector<string> {
                    "Randomly select them from the database",
                    "Specify each object (NOTE: since each transaction defines 4 operations, this becomes tedious with"
                        " more than a few objects)"
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
                int key, value;
                int readOpCount;
                int writeOpCount;
                Record *record;
                mt19937 gen;
                gen.seed(random_device()());
                uniform_real_distribution<> keyDist(0, dataManager->getDb().size() - 1);
                readOnlyOps.clear();
                readWriteOps.clear();

                if (specifyDataObjects) {
                    for (int i = 0; i < readOnlyCount * opPerTxn; i++) {
                        if(i % opPerTxn == 0) {
                            cout << "Transaction " << i + 1 << ":" << endl;
                        }
                        ss << "Which data entry key would you like to perform read operation " << i % opPerTxn + 1 << " on?\n"
                            << "(NOTE: If the entry key specified is for an older/invalid version of an object, HeckaDBMS "
                            "will automatically find the latest version and point your request at it)";
                        prompt = ss.str();
                        key = Utility::PromptUser(prompt, 0, dataManager->getDb().size() - 1);
                        readOnlyOps[i] = Operation();
                        readOnlyOps[i].setMode(Operation::READ);
                        record = dataManager->getDb()[key];
                        while(!record->getIsLatest()) {
                            record = record->getNextRecord();
                        }
                        readOnlyOps[i].setKey(record->getEntryKey());
                    }
                    for (int i = 0; i < readWriteCount * opPerTxn; i++) {
                        if(i % dataManager->getOpsPerTransaction() == 0) {
                            cout << "Transaction " << i + 1 << ":" << endl;
                            readOpCount = 0;
                            writeOpCount = 0;
                        }

                        if(readOpCount < 2 && writeOpCount < 2) {
                            ss << "Would you like operation " << i << " to be a read or a write?";
                            prompt = ss.str();
                            options = vector<string> {"Read", "Write"};
                            responseValue = Utility::PromptUser(prompt, options);
                        }
                        else if(readOpCount < 2) {
                            responseValue = 1;
                        }
                        else {
                            responseValue = 2;
                        }

                        readWriteOps[i] = Operation();
                        if(responseValue == 1) {
                            ss << "Which data entry key would you like to perform read operation " << i << " on?\n"
                               << "(NOTE: If the entry key specified is for an older/invalid version of an object, HeckaDBMS "
                                       "will automatically find the latest version and point your request at it)";
                            readWriteOps[i].setMode(Operation::READ);
                            prompt = ss.str();
                            key = Utility::PromptUser(prompt, 0, dataManager->getDb().size() - 1);
                        }
                        else {
                            ss << "Which data entry key would you like to perform write operation " << i << " on?\n"
                               << "(NOTE: If the entry key specified is for an older/invalid version of an object, HeckaDBMS "
                                       "will automatically find the latest version and point your request at it)";
                            prompt = ss.str();
                            key = Utility::PromptUser(prompt, 0, dataManager->getDb().size() - 1);
                            ss << "What value would you like to write for write operation " << i << "?\n";
                            prompt = ss.str();
                            value = Utility::PromptUser(prompt, 0, 1000);
                            readWriteOps[i].setMode(Operation::WRITE);
                            readWriteOps[i].setValue(value);
                        }
                        record = dataManager->getDb()[key];
                        while(!record->getIsLatest()) {
                            record = record->getNextRecord();
                        }
                        readWriteOps[i].setKey(record->getEntryKey());
                    }
                }
                else {
                    readOnlyOps = Utility::getRandomReadOnlyOps(dataManager, readOnlyCount * opPerTxn);
                    readWriteOps = Utility::getRandomReadWriteOps(dataManager, readWriteCount * opPerTxn);
                }

                _state = BEGIN_TRANSACTION;
                break;
            }
            case BEGIN_TRANSACTION: {
                int j;
                cout << "HeckaDBMS will now begin performing the transaction(s) with the following parameters:" << endl;
                cout << "\t|Read-Only Transactions: " << readOnlyCount << endl;
                cout << "\t|Read-Only Data Object Keys: ";
                for(int i = 0; i < min(readOnlyCount, dataManager->getOpsPrintLim()); i++) {
                    cout << "[";
                    if(i == dataManager->getOpsPrintLim() - 1 && readOnlyCount != dataManager->getOpsPrintLim()) {
                        for(j = 0; j < 3; j++) {
                            cout << readOnlyOps.at(i * opPerTxn + j).getKey() << ",";
                        }
                        cout << readOnlyOps.at(i * opPerTxn + j).getKey() << "] ...";
                    }
                    else if(i == readOnlyCount - 1) {
                        for(j = 0; j < 3; j++) {
                            cout << readOnlyOps.at(i * opPerTxn + j).getKey() << ",";
                        }
                        cout << readOnlyOps.at(i * opPerTxn + j).getKey() << "]";
                    }
                    else {
                        for(j = 0; j < 3; j++) {
                            cout << readOnlyOps.at(i * opPerTxn + j).getKey() << ",";
                        }
                        cout << readOnlyOps.at(i * opPerTxn + j).getKey() << "], ";
                    }
                }
                cout << endl;
                cout << "\t|Read-Write Transactions: " << readWriteCount << endl;
                cout << "\t|Read-Write Data Object Keys: ";
                for(int i = 0; i < min(readWriteCount, dataManager->getOpsPrintLim()); i++) {
                    cout << "[";
                    if(i == dataManager->getOpsPrintLim() - 1 && readWriteCount != dataManager->getOpsPrintLim()) {
                        for(j = 0; j < 3; j++) {
                            if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                                cout << readWriteOps.at(i * opPerTxn + j).getKey() << ",";
                            }
                            else {
                                cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                << readWriteOps.at(i * opPerTxn + j).getValue() << "),";
                            }
                        }
                        if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                            cout << readWriteOps.at(i * opPerTxn + j).getKey() << "] ...";
                        }
                        else {
                            cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                 << readWriteOps.at(i * opPerTxn + j).getValue() << ")] ...";
                        }
                    }
                    else if(i == readWriteCount - 1) {
                        for(j = 0; j < 3; j++) {
                            if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                                cout << readWriteOps.at(i * opPerTxn + j).getKey() << ",";
                            }
                            else {
                                cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                     << readWriteOps.at(i * opPerTxn + j).getValue() << "),";
                            }
                        }
                        if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                            cout << readWriteOps.at(i * opPerTxn + j).getKey() << "]";
                        }
                        else {
                            cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                 << readWriteOps.at(i * opPerTxn + j).getValue() << ")]";
                        }
                    }
                    else {
                        for(j = 0; j < 3; j++) {
                            if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                                cout << readWriteOps.at(i * opPerTxn + j).getKey() << ",";
                            }
                            else {
                                cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                     << readWriteOps.at(i * opPerTxn + j).getValue() << "),";
                            }
                        }
                        if(readWriteOps.at(i * opPerTxn + j).getMode() == Operation::READ) {
                            cout << readWriteOps.at(i * opPerTxn + j).getKey() << "], ";
                        }
                        else {
                            cout << "(" << readWriteOps.at(i * opPerTxn + j).getKey() << ","
                                 << readWriteOps.at(i * opPerTxn + j).getValue() << ")], ";
                        }
                    }
                }
                cout << endl;
                cout << "\t|Concurrent Threads: " << threadCount << endl;
                cout << "Launching threads...\n" << endl;
                transactionManager->manageManualTransactions(dataManager, threadCount, readOnlyCount, readWriteCount, readOnlyOps, readWriteOps, isHekaton);

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

				cout << "Number of Aborts: "<< dataManager->getAbortCounter() << endl;
				cout << "Number of Commits: "<< dataManager->getCommitCounter() << "\n" << endl;

				dataManager->resetAbortCounter();
				dataManager->resetCommitCounter();   
				
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