#ifndef HECKADBMS_DATAMANAGER_H
#define HECKADBMS_DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>
#include <boost/atomic.hpp>
#include "Transaction.h"

#include "Record.h"

using namespace std;

class DataManager
{
public:
    /// Default constructor
    DataManager();

    /// Get the underlying database object
    unordered_map<int, Record *> getDb();

    /// Get a new entry key from a global key manager
    int getLatestEntryKey();

    /// Get a new counter value from a global counter timer
    int getLatestCounter();

    /// Get the number of operations per transaction attempt
    int getOpsPerTransaction();

    /// Get the limit on the number of operations to print in ManualTes
    int getOpsPrintLim();

    /// Set the underlying database object
    void setDb(const unordered_map<int, Record *>& db);

    /// Generate data objects for a database of the specified size
    void generateDatabase(int databaseSize);

    /// Load the data objects from specified filename into memory
    void loadDatabase(string filename);

    /// Save the data objects from memory to the specified filename on disk
    void saveDatabase(string filename);

    /// Clear the current contents of the database
    void clearDatabase();

    /// Print the current contents of the database in a compact format
    void printDatabaseCompact();

    /// Print the current contents of the database in a verbose format
    void printDatabaseVerbose();

    /// Facilitates performing a read on the specified data item through 2PL
    void get();
    
    /// Facilitates performing a read on the specified data item through Hekaton
    void get(int entryKey, unordered_map<int, Transaction *> *transactions, int currTransactionId, vector<Record *> *readSet);

    /// Facilitates performing a write on the specified data item through 2PL
    void put();
    
    /// Facilitates performing a write on the specified data item through Hekaton
    bool put(int entryKey, int value, unordered_map<int, Transaction *> *transactions, int currTransactionId,
         vector<Record *> *writeSet);

private:
    unordered_map<int, Record *> _db;
    boost::atomic<int> _latestEntryKey;
    boost::atomic<int> _latestCounter;
    mutex _putMtx;
    int _opsPerTransaction;
};

#endif
