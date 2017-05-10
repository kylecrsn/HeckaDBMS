#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>

#include "DBMS.h"


DBMS::DBMS()
{

}

DBMS::~DBMS()
{

}

void DBMS::GenerateDataSet(int dbSize)
{
    // Setup random number generator
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> values(0,100);

    // Populate database object
    for(int i = 0; i < dbSize; i++)
    {
        _db.emplace(i, values(rng));
    }
}

// Load the key-values pairs from file back into the db object
void DBMS::LoadDataSet()
{
    std::fstream fileStream;
    std::stringstream datastream;
    std::string line;
    std::string delim = ",";
    int dbSize;
    int key;
    int value;

    // Read the file contents into a string stream
    fileStream.open(_dbFilename, std::fstream::in);
    datastream << fileStream.rdbuf();
    fileStream.close();

    // Determine the db's size
    std::getline(datastream, line);
    dbSize = std::stoi(line);

    // Parse out the key-value pairs
    for(int i = 0; i < dbSize; i++)
    {
        std::getline(datastream, line);
        key = std::stoi(line.substr(0, line.find(delim)));
        value = std::stoi(line.substr(line.find(delim) + 1, line.length()));
        _db.emplace(key, value);
    }
}

// Save the key-values pairs in the current db object to a specified file in .csv format
void DBMS::SaveDataSet()
{
    std::fstream fileStream;
    std::string data;

    // Append the contents of the database to a string
    data.append(std::to_string(_db.size()));
    data.append("\n");

    for(auto i : _db)
    {
        data.append(std::to_string(i.first));
        data.append(",");
        data.append(std::to_string(i.second));
        data.append("\n");
    }

    // Write it out to disk for later use
    fileStream.open(_dbFilename, std::fstream::out | std::fstream::trunc);
    fileStream << data;
    fileStream.close();
}

void DBMS::Get() {

}

void DBMS::Put()
{

}

