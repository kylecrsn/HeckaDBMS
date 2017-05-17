#include <iostream>
#include "unistd.h"
#include <getopt.h>
#include <algorithm>

//using namespace std;

int main(int argc, char *argv[])
{
    int optIndex;
    int c;
    opterr = 0;
    std::string eVal;
    std::string pVal;
    std::string tVal;
    std::string usage = "Usage: user@host $ ./heckadbms -e (client|database) -p (2pl|hekaton) [-t (manual|scale|vary)]";
    static struct option longOptions[] =
        {
            {"execution_type",  required_argument,  0,  'e'},
            {"protocol_type",   required_argument,  0,  'p'},
            {"test_metric",     required_argument,  0,  't'},
            {0, 0, 0, 0}
        };

    // Parse command-line arguments
    while(1)
    {
        // Initialize the index and c
        optIndex = 0;
        c = getopt_long(argc, argv, "e:p:t:", longOptions, &optIndex);

        // Make sure the end hadn't been reached
        if(c == -1)
        {
            break;
        }

        // Cycle through the arguments
        switch(c)
        {
            case 'e':
            {
                eVal = optarg;
                break;
            }
            case 'p':
            {
                pVal = optarg;
                break;
            }
            case 't':
            {
                tVal = optarg;
                break;
            }
            case '?':
            {
                std::cout << usage << std::endl;
                exit(1);
            }
            default:
            {
                std::cout << usage << std::endl;
                exit(1);
            }
        }
    }

    // Post-parsing error handling
    if(eVal == "" || pVal == "")
    {
        std::cout << usage << std::endl;
        exit(1);
    }
    transform(eVal.begin(), eVal.end(), eVal.begin(), ::tolower);
    transform(pVal.begin(), pVal.end(), pVal.begin(), ::tolower);
    if(eVal != "client" && eVal != "database")
    {
        std::cout << "Error: invalid execution_type parameter: " << eVal << std::endl;
        std::cout << usage << std::endl;
        exit(1);
    }
    if(pVal != "2pl" && pVal != "hekaton") {
        std::cout << "Error: invalid protocol_type parameter: " << pVal << std::endl;
        std::cout << usage << std::endl;
        exit(1);
    }
    if(tVal == "")
    {
        tVal = "manual";
    }
    else
    {
        transform(tVal.begin(), tVal.end(), tVal.begin(), ::tolower);
        if(tVal != "manual" && tVal != "scale" && tVal != "vary")
        {
            std::cout << "Error: invalid protocol_type parameter: " << tVal << std::endl;
            std::cout << usage << std::endl;
            exit(1);
        }
    }

    // Check options
    std::cout << "options:" << std::endl << std::endl;
    std::cout << "execution_type: " << eVal << std::endl;
    std::cout << "protocol_type: " << pVal << std::endl;
    std::cout << "test_metric: " << tVal << std::endl;

    exit(0);
}