#include <iostream>

#include "Utility.h"
#include "Client.h"
#include "DataManager.h"

using namespace std;

int main(int argc, char *argv[])
{
    string protocolType;
    Client client = Client();
    DataManager dataManager;

    // Parse command-line arguments
    Utility::ParseArgs(argc, argv, &protocolType);

    // Launch client
    client.FSM(dataManager, protocolType);

    // Examples to demonstrate DM functionality
    /*
    dataManager.GenerateDataSet(15);
    dataManager.PrintDataSet();
    dataManager.SaveDataSet();
    cout << endl << endl;
    dataManager.ClearDataSet();
    dataManager.LoadDataSet();
    dataManager.PrintDataSet();
     */

    exit(0);
}