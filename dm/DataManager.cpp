#include "DataManager.h"

DataManager::DataManager() {
    _db = {};
}

const unordered_map<int, Record>& DataManager::getDb() {
    return _db;
}

void DataManager::setDb(const unordered_map<int, Record>& db) {
    _db = db;
}

void DataManager::generateDatabase(int databaseSize) {
    mt19937 gen;
    gen.seed(random_device()());
    uniform_int_distribution<> objectValueDistribution(0, 1000);

    // Populate the database, initially the entry and object keys are the same
    for(int i = 0; i < databaseSize; i++) {
        Record entry(i, i, objectValueDistribution(gen));
        _db[i] = entry;
    }
}

void DataManager::loadDatabase(string filename) {
    fstream fileStream;
    stringstream datastream;
    string line;
    string delim = ",";
    int entryKey;
    int nextRecordKey;
    int token;
    vector<int> temp(2);
    vector<vector<int>> nextRecordKeys;
    Record entry;

    // Read the file contents into a string stream
    fileStream.open(filename, fstream::in);
    datastream << fileStream.rdbuf();
    fileStream.close();

    // Parse out the key-value pairs
    while(getline(datastream, line)) {
        // Entry key, is latest, object key/value
        entry.setEntryKey(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        token = stoi(line.substr(0, line.find(delim)));
        if(token) {
            entry.setIsLatest(true);
        }
        else {
            entry.setIsLatest(false);
        }
        line.erase(0, line.find(delim) + delim.length());
        entry.setObjectKey(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        entry.setObjectValue(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());

        // Begin timestamp
        token = stoi(line.substr(0, line.find(delim)));
        if(token) {
            entry.getBegin().setIsCounter(true);
        }
        else {
            entry.getBegin().setIsCounter(false);
        }
        line.erase(0, line.find(delim) + delim.length());
        entry.getBegin().setCounter(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        entry.getBegin().setTransactionId(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());

        // End timestamp
        token = stoi(line.substr(0, line.find(delim)));
        if(token) {
            entry.getEnd().setIsCounter(true);
        }
        else {
            entry.getEnd().setIsCounter(false);
        }
        line.erase(0, line.find(delim) + delim.length());
        entry.getEnd().setCounter(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        entry.getEnd().setTransactionId(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());

        // Next record key
        temp.push_back(entry.getEntryKey());
        temp.push_back(stoi(line.substr(0, line.find(delim))));
        nextRecordKeys.push_back(temp);
        temp.clear();

        // Add to the database
        _db[entry.getEntryKey()] = entry;
    }

    // Setup nextRecord pointers
    for(vector<vector<int>>::iterator it = nextRecordKeys.begin(); it != nextRecordKeys.end(); it++) {
        vector<int>::iterator jt = (*it).begin();
        entryKey = (*jt++);
        nextRecordKey = (*jt);
        if(nextRecordKey == -1) {
            _db[entryKey].setNextRecord(nullptr);
        }
        else {
            _db[entryKey].setNextRecord(&_db[nextRecordKey]);
        }
    }
}

void DataManager::saveDatabase(string filename) {
    fstream fileStream;
    string data;
    string delim = ",";

    // Append the contents of each entry in the database
    for(auto i : _db) {
        data.append(to_string(i.second.getEntryKey()));
        data.append(delim);
        if(i.second.getIsLatest()) {
            data.append("1");
        }
        else {
            data.append("0");
        }
        data.append(delim);
        data.append(to_string(i.second.getObjectKey()));
        data.append(delim);
        data.append(to_string(i.second.getObjectValue()));
        data.append(delim);
        if(i.second.getBegin().getIsCounter()) {
            data.append("1");
        }
        else {
            data.append("0");
        }
        data.append(delim);
        data.append(to_string(i.second.getBegin().getCounter()));
        data.append(delim);
        data.append(to_string(i.second.getBegin().getTransactionId()));
        data.append(delim);
        if(i.second.getEnd().getIsCounter()) {
            data.append("1");
        }
        else {
            data.append("0");
        }
        data.append(delim);
        data.append(to_string(i.second.getEnd().getCounter()));
        data.append(delim);
        data.append(to_string(i.second.getEnd().getTransactionId()));
        data.append(delim);
        if(i.second.getNextRecord() == nullptr) {
            data.append("-1");
        }
        else {
            data.append(to_string(i.second.getNextRecord()->getEntryKey()));
        }
        data.append("\n");
    }

    // Write it out to disk for later use
    fileStream.open(filename, fstream::out | fstream::trunc);
    fileStream << data;
    fileStream.close();
}

void DataManager::clearDatabase() {
    _db.clear();
}

void DataManager::printDatabase() {
    string delim = ",";

    for(auto i : _db) {
        cout << i.second.getEntryKey() << delim;
        if(i.second.getIsLatest()) {
            cout << "1" << delim;
        }
        else {
            cout << "0" << delim;
        }
        cout << i.second.getObjectKey() << delim << i.second.getObjectValue() << delim;
        if(i.second.getBegin().getIsCounter()) {
            cout << "1" << delim;
        }
        else {
            cout << "0" << delim;
        }
        cout << i.second.getBegin().getCounter() << delim << i.second.getBegin().getTransactionId() << delim;
        if(i.second.getEnd().getIsCounter()) {
            cout << "1" << delim;
        }
        else {
            cout << "0" << delim;
        }
        cout << i.second.getEnd().getCounter() << delim << i.second.getEnd().getTransactionId() << delim;
        if(i.second.getNextRecord() == nullptr) {
            cout << "-1" << "\n";
        }
        else {
            cout << i.second.getNextRecord()->getEntryKey() << "\n";
        }
    }
    cout << flush;
}

Record DataManager::get(int entryKey) {
    return _db[entryKey];
}

void DataManager::put(Record entry) {
    _db[entry.getEntryKey()] = entry;
}