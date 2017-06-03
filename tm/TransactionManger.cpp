#include "TransactionManager.h"

TransactionManager::TransactionManager() {
    _idCounter = 0;
    _concurrentThreads = 0;
    _transactions = {};
}

int TransactionManager::createId() {
    return ++_idCounter;
}

vector<int> TransactionManager::manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {
    vector<future<int>> listenerThreads(readOnlyCount + readWriteCount);
    vector<int> listenerRets(readOnlyCount + readWriteCount);

    // Launch thread futures
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads[i] = async(&TransactionManager::manualListener, this, dataManager, threadCount, readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys);
    }

    // Get async promise results
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
    }

    return listenerRets;
}

int TransactionManager::manualListener(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {

    while(true) {
        _concurrentMutex.lock();
        if(_concurrentThreads < threadCount) {
            _concurrentThreads++;
            _concurrentMutex.unlock();
            startTransaction(dataManager, readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys);
            _concurrentMutex.lock();
            _concurrentThreads--;
            _concurrentMutex.unlock();
            break;
        }
        else {
            _concurrentMutex.unlock();
        }
    }

    return 42;
}

vector<int> TransactionManager::manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, Utility::ScaleAlgorithm scaleAlgorithm) {

}

vector<int> TransactionManager::manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage) {

}

void TransactionManager::runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly) {
//     int id = createId();
//     _transactions[id] = new Transaction();
	bool notAborted = true;
    Hekaton *h = new Hekaton();
    h->beginTransaction(db, &_transactions);
    h->read(db, &reads, &_transactions);
    if (!readOnly) {
        notAborted = h->write(db, &writes, &_transactions);
    }
    if (notAborted) {
    	h->endNormalProcessing(db, &_transactions, readOnly);
    }
}

Transaction * TransactionManager::createTransactions(int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {
	mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution(0,1000000);
    uniform_real_distribution<> distribution2(0,1);
    int j;
    int readCount;
    int writeCount;
    int w;
	if ((int)distribution2(gen)) {
            if (readOnlyCount != 0) {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(*(readOnlyKeys.begin()));
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                readOnlyCount--;
                //runTransaction(db, reads, vector<pair<int, int>>(), true);
                Transaction *t = new Transaction(reads, vector<pair<int, int>>(), true);
                _transactions[createId()] = t;
            }
            else {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                readWriteCount--;
                //runTransaction(db, reads, writes, false);
                Transaction *t = new Transaction(reads, writes, false);
                _transactions[createId()] = t;
            }
        }
        else {
            if (readWriteCount != 0) {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                readWriteCount--;
                //runTransaction(db, reads, writes, false);
                Transaction *t = new Transaction(reads, writes, false);
                _transactions[createId()] = t;
            }
            else {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(*(readOnlyKeys.begin()));
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                readOnlyCount--;
                //runTransaction(db, reads, vector<pair<int, int>>(), true);
                Transaction *t = new Transaction(reads, vector<pair<int, int>>(), true);
                _transactions[createId()] = t;
            }
        }
}

void TransactionManager::startTransaction(DataManager *db, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {
    for (int i = 0; i < readOnlyCount + readWriteCount; i++) {
    	//lock createTransactions
       Transaction *t = createTransactions(readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys);
       runTransaction(db, t->getReads(), t->getWrites(), t->getIsReadOnly());
    }
}
