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

    DataManager *dataManager = new DataManager();
    TransactionManager *transactionManager = new TransactionManager();

    cout << "==================================================" << endl;
    cout << "||             WELCOME TO HeckaDBMS             ||" << endl;
    cout << "==================================================\n" << endl;

    client.FSM(dataManager, transactionManager, true);

    cout << "==================================================" << endl;
    cout << "||               EXITING HeckaDBMS              ||" << endl;
    cout << "==================================================\n" << endl;

    exit(0);
}
