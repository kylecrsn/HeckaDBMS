#include <iostream>

#include "DataManager.h"
#include "TransactionManager.h"
#include "Utility.h"
#include "Client.h"

using namespace std;

int main(int argc, char *argv[])
{
    string protocolType;
    bool isHekaton = false;
    Client client = Client();

    DataManager *dataManager = new DataManager();
    TransactionManager *transactionManager = new TransactionManager();


    // Parse command-line arguments
    Utility::ParseArgs(argc, argv, &protocolType);
    if(protocolType == "hekaton") {
        isHekaton = true;
    }

    cout << "==================================================" << endl;
    cout << "||             WELCOME TO HeckaDBMS             ||" << endl;
    cout << "==================================================\n" << endl;

    // Launch client
    client.FSM(dataManager, transactionManager, isHekaton);

    cout << "==================================================" << endl;
    cout << "||               EXITING HeckaDBMS              ||" << endl;
    cout << "==================================================\n" << endl;

    exit(0);
}
