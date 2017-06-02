#include <iostream>

#include "DataManager.h"
#include "TransactionManager.h"
#include "Utility.h"
#include "Client.h"

using namespace std;

int main(int argc, char *argv[])
{
    string protocolType;
    Client client = Client();
    DataManager *dataManager;
    TransactionManager *transactionManager;

    // Parse command-line arguments
    Utility::ParseArgs(argc, argv, &protocolType);

    cout << "==================================================" << endl;
    cout << "||             WELCOME TO HeckaDBMS             ||" << endl;
    cout << "==================================================\n" << endl;

    // Launch client
    client.FSM(dataManager, transactionManager);

    cout << "==================================================" << endl;
    cout << "||               EXITING HeckaDBMS              ||" << endl;
    cout << "==================================================\n" << endl;

    exit(0);
}