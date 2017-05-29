#include "TransactionManager.h"

TransactionManager::TransactionManager() {
    _idCounter = 0;
    _transactions = {};
}

int TransactionManager::createId() {
    mutex mtx;
    int newId;

    mtx.lock();
    newId = _idCounter++;
    mtx.unlock();

    return newId;
}

void TransactionManager::runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly) {
    int id = createId();
    _transactions[id] = new Transaction();
    Hekaton *h = new Hekaton();
    h->beginTransaction(db, &_transactions);
    h->read(db, reads, &_transactions);
    if (!readOnly) {
        h->write(db, writes, &_transactions);
        h->endNormalProcessing(db, &_transactions);
    }
}

void TransactionManager::createTransactions(DataManager *db, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys, int threadCount) {
    mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution(0,1000000);
    int readOnlyLeft;
    int readWriteLeft;
    int j;
    int readCount;
    int writeCount;
    srand(time(NULL));
    for (int i = 0; i < readOnlyCount + readWriteCount; i++) {
        if (rand() % 2) {
            if (readOnlyCount != 0) {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(readOnlyKeys.front());
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                readOnlyCount --;
                runTransaction(db, reads, NULL, true);
            }
            else {
                vector<pair<int,int> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if (rand() % 2) {
                        if (readCount !=2) {
                            reads.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            writes.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
                            writes.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            reads.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                readWriteCount--;
                runTransaction(db, reads, writes, false);
            }
        }
        else {
            if (readWriteCount != 0) {
                vector<pair<int,int> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if (rand() % 2) {
                        if (readCount !=2) {
                            reads.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            writes.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
                            writes.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            reads.push_back(readWriteKeys.front());
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                readWriteCount--;
                runTransaction(db, reads, writes, false);
            }
            else {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(readOnlyKeys.front());
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                readOnlyCount--;
                runTransaction(db, reads, NULL, true);
            }
        }
    }
}