#include "DataManager.h"

DataManager::DataManager() {
    _db = {};
    _latestCounter = 0;
    _latestEntryKey = 0;
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
            entry.getBegin()->setIsCounter(true);
        }
        else {
            entry.getBegin()->setIsCounter(false);
        }
        line.erase(0, line.find(delim) + delim.length());
        entry.getBegin()->setCounter(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        entry.getBegin()->setTransactionId(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());

        // End timestamp
        token = stoi(line.substr(0, line.find(delim)));
        if(token) {
            entry.getEnd()->setIsCounter(true);
        }
        else {
            entry.getEnd()->setIsCounter(false);
        }
        line.erase(0, line.find(delim) + delim.length());
        entry.getEnd()->setCounter(stoi(line.substr(0, line.find(delim))));
        line.erase(0, line.find(delim) + delim.length());
        entry.getEnd()->setTransactionId(stoi(line.substr(0, line.find(delim))));
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
        if(i.second.getBegin()->getIsCounter()) {
            data.append("1");
        }
        else {
            data.append("0");
        }
        data.append(delim);
        data.append(to_string(i.second.getBegin()->getCounter()));
        data.append(delim);
        data.append(to_string(i.second.getBegin()->getTransactionId()));
        data.append(delim);
        if(i.second.getEnd()->getIsCounter()) {
            data.append("1");
        }
        else {
            data.append("0");
        }
        data.append(delim);
        data.append(to_string(i.second.getEnd()->getCounter()));
        data.append(delim);
        data.append(to_string(i.second.getEnd()->getTransactionId()));
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
        if(i.second.getBegin()->getIsCounter()) {
            cout << "1" << delim;
        }
        else {
            cout << "0" << delim;
        }
        cout << i.second.getBegin()->getCounter() << delim << i.second.getBegin()->getTransactionId() << delim;
        if(i.second.getEnd()->getIsCounter()) {
            cout << "1" << delim;
        }
        else {
            cout << "0" << delim;
        }
        cout << i.second.getEnd()->getCounter() << delim << i.second.getEnd()->getTransactionId() << delim;
        if(i.second.getNextRecord() == nullptr) {
            cout << "-1" << "\n";
        }
        else {
            cout << i.second.getNextRecord()->getEntryKey() << "\n";
        }
    }
    cout << flush;
}

int DataManager::getLatestEntryKey() {
	mutex mtx;
	mtx.lock();
	_latestEntryKey++;
	int newEntryKey = _latestEntryKey;
	mtx.unlock();
	return newEntryKey;
}

int DataManager::getLatestCounter() {
	mutex mtx;
	mtx.lock();
	_latestCounter++;
	int newCounter = _latestCounter;
	mtx.unlock();
	return newCounter;
}

void DataManager::get(int entryKey, unordered_map<int, Transaction *> *transactions, int currTransactionId, vector<Record *> *readSet) {
	//Record record = _db[entryKey];
	Record *record;
	Transaction *currTransaction = transactions->at(currTransactionId);
	Transaction *transaction;
	while (record) {
			//begin and end timestamps are set
			if (record->getBegin()->getIsCounter() && record->getEnd()->getIsCounter() && record->getBegin()->getCounter() != -1) {
				if (record->getBegin()->getCounter() < currTransaction->getBegin()->getCounter() && record->getEnd()->getCounter() == -1) { //end timestamp must be infinity?
					readSet->push_back(record);
					break;
				}
				else {
					record = record->getNextRecord();
				}
			}
			//begin timestamp is transaction ID
			else if (!record->getBegin()->getIsCounter()) {
				transaction = transactions->at(record->getBegin()->getTransactionId());
				if (record->getBegin()->getTransactionId() == currTransactionId && transaction->getState() == 1 && transaction->getEnd()->getIsCounter() && transaction->getEnd()->getCounter() == -1) {
					readSet->push_back(record);
					break;
				}
				else if (transaction->getState() == 2 && transaction->getEnd()->getCounter() < currTransaction->getBegin()->getCounter()) {
					readSet->push_back(record);
					currTransaction->setCommitDepCounter(currTransaction->getCommitDepCounter()+1);
					transaction->getCommitDepSet()->push_back(currTransactionId);
					break;
				}
				else if (transaction->getState() == 3 && transaction->getEnd()->getCounter() < currTransaction->getBegin()->getCounter()) {
					readSet->push_back(record);
					break;
				}
				//not sure what to do about terminated transaction
			}
			//end timestamp is transaction ID
			else if (!record->getEnd()->getIsCounter()) {
				transaction = transactions->at(record->getEnd()->getTransactionId());
				if (transaction->getState() == 2 && transaction->getEnd()->getCounter() > currTransaction->getBegin()->getCounter()) {
					readSet->push_back(record);
				}
				else if (transaction->getState() == 2 && transaction->getEnd()->getCounter() < currTransaction->getBegin()->getCounter()) {
					currTransaction->setCommitDepCounter(currTransaction->getCommitDepCounter()+1);
					transaction->getCommitDepSet()->push_back(currTransactionId);
					record = record->getNextRecord(); //speculatively ignore?
				}
				else if (transaction->getState() == 3 && transaction->getEnd()->getCounter() > currTransaction->getBegin()->getCounter()) {
					readSet->push_back(record);
					break;
				}
				else if (transaction->getState() == 4) {
					readSet->push_back(record);
					break;
				}
				//not sure what to do about terminated transaction
			}
	}
}

void DataManager::put() {

}

bool DataManager::put(int entryKey, int value, unordered_map<int, Transaction *> *transactions, int currTransactionId, vector<Record *> *writeSet) {
		mutex mtx;
		//Record *record = _db[entryKey];
		Record *record;
		while (record) {
			if (record->getEnd()->getIsCounter() && record->getEnd()->getCounter() == -1) {
				Timestamp *tBegin = new Timestamp(false, 0, currTransactionId);
				Timestamp *tEnd = new Timestamp(true, -1, currTransactionId);
				Record *newRecord = new Record(tBegin, tEnd, getLatestEntryKey(), record->getObjectKey(), value);
				mtx.lock();
				if (record->getEnd()->getIsCounter() && record->getEnd()->getCounter() == -1) {
					record->getEnd()->setTransactionId(currTransactionId);
				}
				else {
					return 0;
				}
				mtx.unlock();
				record->getEnd()->setIsCounter(false);
				record->setNextRecord(newRecord);
				writeSet->push_back(newRecord);
				writeSet->push_back(record);
			}
			else if (!record->getEnd()->getIsCounter()) {
				int transactionId = record->getEnd()->getTransactionId();
				int state = transactions->at(transactionId)->getState();
				//transaction is aborted so you can update
				if (state == 4) {
					Timestamp *tBegin = new Timestamp(false, 0, currTransactionId);
					Timestamp *tEnd = new Timestamp(true, -1, currTransactionId);
					Record *newRecord = new Record(tBegin, tEnd, getLatestEntryKey(), record->getObjectKey(), value);
					mtx.lock();
					if (record->getEnd()->getTransactionId() == transactionId) {
						record->getEnd()->setTransactionId(currTransactionId);
					}
					else {
						return 0;
					}
					mtx.unlock();
					record->getEnd()->setIsCounter(false);
					record->setNextRecord(newRecord);
					writeSet->push_back(newRecord);
					writeSet->push_back(record);
				}
				else {
					return 0;
				}
			}
		}
}


