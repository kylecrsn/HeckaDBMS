#ifndef HECKADBMS_DATAMANAGER_H
#define HECKADBMS_DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>

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

    /// Print the current contents of the database (intended for debugging purposes)
    void printDatabase();

    /// Facilitates performing a read on the specified data object
    Record get(int entryKey);

    /// Facilitates performing a write on the specified data object
    void put(Record entry);

private:
    unordered_map<int, Record> _db;
};

#endif
