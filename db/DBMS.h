#ifndef HECKADBMS_DBMS_H
#define HECKADBMS_DBMS_H

#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class DBMS
{
public:

    // Default constructor
    DBMS();

    // Destructor
    ~DBMS();

    // Facilitates performing a read on the specified data item
    void Get();

    // Facilitates performing a write on the specified data item
    void Put();

private:

    // Generate a database key-value set with keys 0 - size-1 and random values 0 - 100
    void GenerateDataSet(int dbSize);

    // Load a data set from disk into memory
    void LoadDataSet();

    // Save a data set from memory to disk
    void SaveDataSet();

    unordered_map<int, int> _db;
    string _dbFilename;

};

#endif
