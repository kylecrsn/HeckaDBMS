#include "TransactionManager.h"

TransactionManager::TransactionManager() {
    _idCounter = 0;
    _concurrentThreads = 0;
    _transactions = {};
    _readOnlyLeft = 0;
    _readWriteLeft = 0;
}

void TransactionManager::setReadOnlyLeft(int l) {
	_readOnlyLeft = l;
}
    
void TransactionManager::setReadWriteLeft(int l) {
	_readWriteLeft = l;
}

int TransactionManager::createId() {
    return ++_idCounter;
}

vector<int> TransactionManager::manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
    vector<future<int>> listenerThreads(readOnlyCount + readWriteCount);
    vector<int> listenerRets(readOnlyCount + readWriteCount);
    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Launch thread futures
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads[i] = async(&TransactionManager::transactionListener, this, dataManager, threadCount, readOnlyOps, readWriteOps);
    }

    // Get async promise results
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
    }

    return listenerRets;
}

vector<int> TransactionManager::manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount) {
    int transactionsCompleted = 0;
    int functionalConcurrentThreads = 1;
    int readOnlyCount = transactionCount / 2;
    int readWriteCount = transactionCount - readOnlyCount;
    vector<Operation> readOnlyOps = Utility::getRandomReadOnlyOps(dataManager, readOnlyCount * dataManager->getOpsPerTransaction());
    vector<Operation> readWriteOps = Utility::getRandomReadWriteOps(dataManager, readWriteCount * dataManager->getOpsPerTransaction());
    vector<future<int>> listenerThreads(transactionCount);
    vector<int> listenerRets(transactionCount);

    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Launch thread futures
    while(transactionsCompleted < transactionCount) {
        for(int i = 0; i < functionalConcurrentThreads; i++) {
            if(i + transactionsCompleted < transactionCount) {
                listenerThreads[i + transactionsCompleted] = async(&TransactionManager::transactionListener, this, dataManager, functionalConcurrentThreads, readOnlyOps, readWriteOps);
            }
        }
        for(int i = 0; i < functionalConcurrentThreads; i++) {
            if(i + transactionsCompleted < transactionCount) {
                listenerRets[i + transactionsCompleted] = listenerThreads[i].get();
            }
        }
        transactionsCompleted += functionalConcurrentThreads;
    }

    return listenerRets;
}

vector<int> TransactionManager::manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage) {
    int readOnlyCount = ((float)roPercentage / 100.0) * transactionCount;
    int readWriteCount = transactionCount - readOnlyCount;
    vector<Operation> readOnlyOps = Utility::getRandomReadOnlyOps(dataManager, readOnlyCount * dataManager->getOpsPerTransaction());
    vector<Operation> readWriteOps = Utility::getRandomReadWriteOps(dataManager, readWriteCount * dataManager->getOpsPerTransaction());
    vector<future<int>> listenerThreads(transactionCount);
    vector<int> listenerRets(transactionCount);

    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Launch thread futures
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads[i] = async(&TransactionManager::transactionListener, this, dataManager, threadCount, readOnlyOps, readWriteOps);
    }

    // Get async promise results
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
    }

    return listenerRets;
}

int TransactionManager::transactionListener(DataManager *dataManager, int threadCount, vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
    while(true) {
        _concurrentMutex.lock();
        if(_concurrentThreads < threadCount) {
            _concurrentThreads++;
            _concurrentMutex.unlock();
            startTransaction(dataManager, readOnlyOps, readWriteOps);
            _concurrentMutex.lock();
            _concurrentThreads--;
            _concurrentMutex.unlock();
            break;
        }
        else {
            _concurrentMutex.unlock();
        }
    }

    //temp
    return 42;
}

void TransactionManager::startTransaction(DataManager *db, vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
    //lock around createTransactions
    _transactionMtx.lock();
    Transaction *t = createTransaction(readOnlyOps, readWriteOps);
    _transactionMtx.unlock();
    runTransaction(db, t->getReads(), t->getWrites(), t->getIsReadOnly(), t->getId());
}

void TransactionManager::runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly, int i) {
//     int id = createId();
//     _transactions[id] = new Transaction();
	bool notAborted = true;
    Hekaton *h = new Hekaton();
    h->setId(i);
    h->beginTransaction(db, &_transactions);
    h->read(db, &reads, &_transactions);
    if (!readOnly) {
        notAborted = h->write(db, &writes, &_transactions);
    }
    if (notAborted) {
    	h->endNormalProcessing(db, &_transactions, readOnly);
    }
}

void TransactionManager::run2PLTransaction(DataManager *db, Transaction *t) {
	LockManager *l = new LockManager();
	for (vector<Operation *>::iterator it = t->getTransaction().begin() ; it != t->getTransaction().end(); ++it) {
		l->Lock((*it)->getObjectKey(), t->getTransactionId(), (*it)->getMode());
		if ((*it)->getMode() == Operation::Mode::WRITE) {
			put((*it)->getObjectKey(), (*it)->getValue());
		}
		else {
			get((*it)->getObjectKey());
		}
	}
	l->Unlock(*t->getTransaction().begin(), t->getTransactionId());
} 

Transaction * TransactionManager::createTransaction(vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
	mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution2(0,1);
    int j;
    int readCount = 0;
    int writeCount = 0;
    int w;
    int id;
    Transaction *t;
	if ((int)distribution2(gen)) {
            if (_readOnlyLeft != 0) {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(readOnlyOps.begin()->getKey());
                    readOnlyOps.erase(readOnlyOps.begin());
                }
                _readOnlyLeft--;
                t = new Transaction(reads, vector<pair<int, int>>(), true);
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
            else {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            reads.push_back(readWriteOps.begin()->getKey());
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                        else {
			  				writes.push_back(make_pair(readWriteOps.begin()->getKey(), readWriteOps.begin()->getValue()));
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteOps.begin()->getKey(), readWriteOps.begin()->getValue()));
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                        else {
                            reads.push_back(readWriteOps.begin()->getKey());
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                    }
                }
                readCount = 0;
                writeCount = 0;
                _readWriteLeft--;
                t = new Transaction(reads, writes, false);
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        else {
            if (_readWriteLeft != 0) {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            reads.push_back(readWriteOps.begin()->getKey());
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                        else {
			  				writes.push_back(make_pair(readWriteOps.begin()->getKey(), readWriteOps.begin()->getValue()));
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteOps.begin()->getKey(), readWriteOps.begin()->getValue()));
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                        else {
                            reads.push_back(readWriteOps.begin()->getKey());
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                    }
                }
                readCount = 0;
                writeCount = 0;
                _readWriteLeft--;
                t = new Transaction(reads, writes, false);
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
            else {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    reads.push_back(readOnlyOps.begin()->getKey());
                    readOnlyOps.erase(readOnlyOps.begin());
                }
                _readOnlyLeft--;
                t = new Transaction(reads, vector<pair<int, int>>(), true);
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        return t;
}


Transaction * TransactionManager::create2PLTransaction(vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
  	mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution2(0,1);
    int j;
    int readCount;
    int writeCount;
    int w;
    int id;
    Transaction *t = new Transaction();
    if ((int)distribution2(gen)) {
            if (_readOnlyLeft != 0) {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    t->addOperation(&readOnlyOps[0]);
                    readOnlyOps.erase(readOnlyOps.begin());
                }
                _readOnlyLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
            else {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                        else {
			  				t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                        else {
                            t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                    }
                }
                readCount = 0;
                writeCount = 0;
                _readWriteLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        else {
            if (_readWriteLeft != 0) {
                vector<pair<int,int>> writes;
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                        else {
			  				t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            writeCount++;
                        }
                        else {
                            t->addOperation(&readWriteOps[0]);
                            readWriteOps.erase(readWriteOps.begin());
                            readCount++;
                        }
                    }
                }
                readCount = 0;
                writeCount = 0;
                _readWriteLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
            else {
                vector<int> reads;
                for (j = 0; j < 4; j++) {
                     t->addOperation(&readOnlyOps[0]);
                    readOnlyOps.erase(readOnlyOps.begin());
                }
                _readOnlyLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        return t;
	}