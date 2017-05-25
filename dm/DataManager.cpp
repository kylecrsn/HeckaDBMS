#include "DataManager.h"

DataManager::DataManager() {
    _db = {};
}

const unordered_map<int, int>& DataManager::GetDB() {
    return _db;
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

bool DataManager::LoadDataSet(string filename, string expectedProtocol) {
    fstream fileStream;
    stringstream datastream;
    string line;
    string fileProtocol;
    string delim = ",";
    int dbSize;
    int key;
    int value;

    // Read the file contents into a string stream
    fileStream.open(filename, fstream::in);
    datastream << fileStream.rdbuf();
    fileStream.close();

    // Determine the db's protocol and size
    getline(datastream, line);
    fileProtocol = line.substr(0, line.find(delim));
    dbSize = stoi(line.substr(line.find(delim) + 1, line.size()));

    if(fileProtocol == expectedProtocol) {
        // Parse out the key-value pairs
        for(int i = 0; i < dbSize; i++) {
            getline(datastream, line);
            key = stoi(line.substr(0, line.find(delim)));
            value = stoi(line.substr(line.find(delim) + 1, line.length()));
            _db[key] = value;
        }

        return true;
    }
    else {
        return false;
    }
}

void DataManager::SaveDataSet(string filename, string currentProtocol) {
    fstream fileStream;
    string data;
    int sizeLength;

    // Append the contents of the database to a string
    data.append(currentProtocol);
    data.append(",");
    data.append(to_string(_db.size()));
    data.append("\n");
    sizeLength = data.size();

    for(auto const &i : _db) {
        data.insert(sizeLength, "\n");
        data.insert(sizeLength, to_string(i.second));
        data.insert(sizeLength, ",");
        data.insert(sizeLength, to_string(i.first));
    }

    // Write it out to disk for later use
    fileStream.open(filename, fstream::out | fstream::trunc);
    fileStream << data;
    fileStream.close();
}

void DataManager::ClearDataSet() {
    _db.clear();
}

void DataManager::PrintDataSet() {
    for(auto const &i : _db) {
        cout << i.first << "," << i.second << "\n";
    }
    cout << flush;
}

void DataManager::Get() {

}

void DataManager::Put() {

}