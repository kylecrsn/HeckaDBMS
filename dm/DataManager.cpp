#include "DataManager.h"

DataManager::DataManager()
{
    _db = {};
    _dbFilename = "";
}

DataManager::DataManager(std::string dbFilename) {
    _db = {};
    _dbFilename = dbFilename;
}

void DataManager::GenerateDataSet(int dbSize)
{
    // Setup random number generator
    mt19937 gen;
    gen.seed(random_device()());
    uniform_int_distribution<> distribution(0,100);

    // Populate database object
    for(int i = 0; i < dbSize; i++)
    {
        _db.emplace(i, distribution(gen));
    }
}

// Load the key-values pairs from file back into the dm object
void DataManager::LoadDataSet()
{
    fstream fileStream;
    stringstream datastream;
    string line;
    string delim = ",";
    int dbSize;
    int key;
    int value;

    // Read the file contents into a string stream
    fileStream.open(_dbFilename, fstream::in);
    datastream << fileStream.rdbuf();
    fileStream.close();

    // Determine the dm's size
    getline(datastream, line);
    dbSize = stoi(line);

    // Parse out the key-value pairs
    for(int i = 0; i < dbSize; i++)
    {
        getline(datastream, line);
        key = stoi(line.substr(0, line.find(delim)));
        value = stoi(line.substr(line.find(delim) + 1, line.length()));
        _db.emplace(key, value);
    }
}

// Save the key-values pairs in the current dm object to a specified file in .csv format
void DataManager::SaveDataSet()
{
    fstream fileStream;
    string data;

    // Append the contents of the database to a string
    data.append(to_string(_db.size()));
    data.append("\n");

    for(auto i : _db)
    {
        data.append(to_string(i.first));
        data.append(",");
        data.append(to_string(i.second));
        data.append("\n");
    }

    // Write it out to disk for later use
    fileStream.open(_dbFilename, fstream::out | fstream::trunc);
    fileStream << data;
    fileStream.close();
}

void DataManager::Get()
{

}

void DataManager::Put()
{

}

