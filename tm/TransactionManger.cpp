#include "TransactionManager.h"

TransactionManager::TransactionManager() {
    _idCounter = 0;
    _concurrentThreads = 0;
    _transactions = {};
}

int TransactionManager::createId() {
    return ++_idCounter;
}

void TransactionManager::manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {
    vector<thread> listenerThreads(readOnlyCount + readWriteCount);

    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads.at(i) = thread(manualListener, dataManager, threadCount, readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys);
    }

    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads.at(i).join();
    }

    //return output data
}

void TransactionManager::manualListener(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys) {

}

void TransactionManager::manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, Utility::ScaleAlgorithm scaleAlgorithm) {

}

void TransactionManager::manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage) {

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

void TransactionManager::createThreads(DataManager *db, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys, int threadCount) {
    for (int i = 0; i < readOnlyCount + readWriteCount; i++) {
    	//lock createTransactions
       Transaction *t = createTransactions(readOnlyCount, readWriteCount, readOnlyKeys, readWriteKeys);
       runTransaction(db, t->getReads(), t->getWrites(), t->getIsReadOnly());
    }
}
