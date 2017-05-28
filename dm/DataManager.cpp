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

void DataManager::Get(int object, unordered_map<int, Transaction *> *transactions, int currTransactionId, vector<Record *> *readSet) {
	//Record *record = _db[object];
	Record *record;
	Transaction *currTransaction = transactions->at(currTransactionId);
	Transaction *transaction;
	while (record) {
			//begin and end timestamps are set
			if (record->getBegin()->isCounter() && record->getEnd()->isCounter() && record->getBegin()->getCounter() != -1) {
				if (record->getBegin()->getCounter() < currTransaction->getBegin()->getCounter() && record->getEnd()->getCounter() == -1) { //end timestamp must be infinity?
					readSet->push_back(record);
					break;
				}
				else {
					record = record->getNextRecord();
				}
			}
			//begin timestamp is transaction ID
			else if (!record->getBegin()->isCounter()) {
				transaction = transactions->at(record->getBegin()->getTransactionId());
				if (record->getBegin()->getTransactionId() == currTransactionId && transaction->getState() == 1 && transaction->getEnd()->isCounter() && transaction->getEnd()->getCounter() == -1) {
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
			else if (!record->getEnd()->isCounter()) {
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

void DataManager::Put() {

}

bool DataManager::Put(int object, int value, unordered_map<int, Transaction *> *transactions, int currTransactionId, vector<Record *> *writeSet) {
	if (_db.find(object) != _db.end()) {
		mutex mtx;
		//Record *record = _db[object];
		Record *record;
		while (record) {
			if (record->getEnd()->isCounter() && record->getEnd()->getCounter() == -1) {
				Timestamp *tBegin = new Timestamp(false, 0, currTransactionId);
				Timestamp *tEnd = new Timestamp(true, -1, currTransactionId);
				Record *newRecord = new Record(tBegin, tEnd, object, value);
				mtx.lock();
				if (record->getEnd()->isCounter() && record->getEnd()->getCounter() == -1) {
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
			else if (!record->getEnd()->isCounter()) {
				int transactionId = record->getEnd()->getTransactionId();
				int state = transactions->at(transactionId)->getState();
				//transaction is aborted so you can update
				if (state == 4) {
					Timestamp *tBegin = new Timestamp(false, 0, currTransactionId);
					Timestamp *tEnd = new Timestamp(true, -1, currTransactionId);
					Record *newRecord = new Record(tBegin, tEnd, object, value);
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
	} else {
		Timestamp *tBegin = new Timestamp(false, 0, currTransactionId);
		Timestamp *tEnd = new Timestamp(true, -1, currTransactionId);
		Record *newRecord = new Record(tBegin, tEnd, object, value);
		//_db[object] = newRecord;
		writeSet->push_back(newRecord);
	}
}





























