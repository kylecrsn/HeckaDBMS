#include <iostream>

#include "Utility.h"
#include "Client.h"

using namespace std;

int main(int argc, char *argv[])
{
    string protocolType;
    string testMetric;
    Client client = Client();

    // Parse command-line arguments
    Utility::ParseArgs(argc, argv, &protocolType, &testMetric);

    // Launch client
    client.FSM();

    exit(0);
}