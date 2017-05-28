#ifndef HECKADBMS_DATAMANAGER_H
#define HECKADBMS_DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>
#include "Transaction.h"

#include "Record.h"

using namespace std;

class DataManager
{
public:
    // Default constructor
    DataManager();

    // Get back the underlying database object
    const unordered_map<int, int>& GetDB();

    // Generate a database key-value set with keys 0 - size-1 and random values 0 - 100
    void GenerateDataSet(int dbSize);

    // Load the key-values pairs from file back into the dm object's memory
    bool LoadDataSet(string filename, string expectedProtocol);

    // Save the key-values pairs in the current dm object to a specified file in .csv format
    void SaveDataSet(string filename, string currentProtocol);

    // Clear the current contents of the database
    void ClearDataSet();

    // Print the database contents to console, intended for debugging purposes, limited to DB's <= 100 entries
    void PrintDataSet();

    // Facilitates performing a read on the specified data item through 2PL
    void Get(int object);
    
    // Facilitates performing a read on the specified data item through Hekaton
    void Get(int object, unordered_map<int, Transaction *> *transactions, int currTransaction, vector<Record *> *readSet);

    // Facilitates performing a write on the specified data item through 2PL
    void Put();
    
    // Facilitates performing a write on the specified data item through Hekaton
    bool Put(int object, int value, unordered_map<int, Transaction *> *transactions, int currTransaction, vector<Record *> *writeSet);

private:
    unordered_map<int, int> _db;
};

#endif
