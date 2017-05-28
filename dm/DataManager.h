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
    /// Default constructor
    DataManager();

    /// Get the underlying database object
    const unordered_map<int, Record>& getDb();

    /// Set the underlying database object
    void setDb(const unordered_map<int, Record>& db);

    /// Generate data objects for a database of the specified size
    void generateDatabase(int databaseSize);

    /// Load the data objects from specified filename into memory
    void loadDatabase(string filename);

    /// Save the data objects from memory to the specified filename on disk
    void saveDatabase(string filename);

    /// Clear the current contents of the database
    void clearDatabase();
    
    int getLatestEntryKey();
    
    int getLatestCounter();

    // Facilitates performing a read on the specified data item through 2PL
    void get(int object);
    
    // Facilitates performing a read on the specified data item through Hekaton
    void get(int object, unordered_map<int, Transaction *> *transactions, int currTransaction, vector<Record *> *readSet);

    // Facilitates performing a write on the specified data item through 2PL
    void put();
    
    // Facilitates performing a write on the specified data item through Hekaton
    bool put(int object, int value, unordered_map<int, Transaction *> *transactions, int currTransaction, vector<Record *> *writeSet);
    /// Print the current contents of the database (intended for debugging purposes)
    void printDatabase();



private:
    unordered_map<int, Record> _db;
    int latestEntryKey;
    int latestCounter;
};

#endif
