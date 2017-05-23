#include "DataManager.h"

DataManager::DataManager() {
    _db = {};
    _dbFilename = "";
}

DataManager::DataManager(std::string dbFilename) {
    _db = {};
    _dbFilename = dbFilename;
}

void DataManager::GenerateDataSet(int dbSize) {
    // Setup random number generator
    mt19937 gen;
    gen.seed(random_device()());
    uniform_int_distribution<> distribution(0,100);

    // Populate database object
    for(int i = 0; i < dbSize; i++) {
        _db[i] = distribution(gen);
    }
}

void DataManager::LoadDataSet() {
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
    dbSize = stoi(line.substr(0, line.find(delim)));

    // Parse out the key-value pairs
    for(int i = 0; i < dbSize; i++) {
        getline(datastream, line);
        key = stoi(line.substr(0, line.find(delim)));
        value = stoi(line.substr(line.find(delim) + 1, line.length()));
        _db[key] = value;
    }
}

void DataManager::SaveDataSet() {
    fstream fileStream;
    string data;
    int sizeLength;

    // Append the contents of the database to a string
    data.append(to_string(_db.size()));
    data.append(",\n");
    sizeLength = data.size();

    for(auto const &i : _db) {
        data.insert(sizeLength, "\n");
        data.insert(sizeLength, to_string(i.second));
        data.insert(sizeLength, ",");
        data.insert(sizeLength, to_string(i.first));
    }

    // Write it out to disk for later use
    fileStream.open(_dbFilename, fstream::out | fstream::trunc);
    fileStream << data;
    fileStream.close();
}

void DataManager::ClearDataSet() {
    _db.clear();
}

void DataManager::PrintDataSet() {
    cout << _db.size() << "," << endl;
    for(auto const &i : _db) {
        cout << i.first << "," << i.second << "\n";
    }
    cout << flush;
}

void DataManager::Get() {

}

void DataManager::Put() {

}