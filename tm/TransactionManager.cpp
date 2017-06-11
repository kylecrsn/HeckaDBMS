#include "TransactionManager.h"

TransactionManager::TransactionManager() {
    _idCounter = -1;
    _concurrentThreads = 0;
    _transactions = unordered_map<int, Transaction *>();
    _readOnlyLeft = 0;
    _readWriteLeft = 0;
    _currId = 0;
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

vector<int> TransactionManager::manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<Operation> readOnlyOps, vector<Operation> readWriteOps, bool isHekaton) {
    vector<future<int>> listenerThreads(readOnlyCount + readWriteCount);
    vector<int> listenerRets(readOnlyCount + readWriteCount);
    _readOnlyOps = &readOnlyOps;
    _readWriteOps = &readWriteOps;

    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Create the necessary transactions
	for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        if(isHekaton) {
            createTransaction();
        }
        else {
            create2PLTransaction();
        }
	}

    // Launch thread futures
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads[i] = async(launch::async, &TransactionManager::transactionListener, this, dataManager, threadCount, isHekaton);
    }

    // Get async promise results
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
    } 

    return listenerRets;
}

vector<int> TransactionManager::manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, bool isHekaton) {
    int transactionsCompleted = 0;
    int functionalConcurrentThreads = 1;
    int readOnlyCount = transactionCount / 2;
    int readWriteCount = transactionCount - readOnlyCount;
    vector<Operation> readOnlyOps = Utility::getRandomReadOnlyOps(dataManager, readOnlyCount * dataManager->getOpsPerTransaction());
    vector<Operation> readWriteOps = Utility::getRandomReadWriteOps(dataManager, readWriteCount * dataManager->getOpsPerTransaction());
    vector<future<int>> listenerThreads(transactionCount);
    vector<int> listenerRets(transactionCount);
    _readOnlyOps = &readOnlyOps;
    _readWriteOps = &readWriteOps;

    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Create the necessary transactions
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        if(isHekaton) {
            createTransaction();
        }
        else {
            create2PLTransaction();
        }
    }

    // Launch thread futures
    while(transactionsCompleted < transactionCount) {
        for(int i = 0; i < functionalConcurrentThreads; i++) {
            if(i + transactionsCompleted < transactionCount) {
                listenerThreads[i + transactionsCompleted] = async(launch::async, &TransactionManager::transactionListener, this, dataManager, functionalConcurrentThreads, isHekaton);
            }
        }
        for(int i = 0; i < functionalConcurrentThreads; i++) {
            if(i + transactionsCompleted < transactionCount) {
                listenerRets[i + transactionsCompleted] = listenerThreads[i].get();
            }
        }
        transactionsCompleted += functionalConcurrentThreads;
        functionalConcurrentThreads++;
    }

    return listenerRets;
}

vector<int> TransactionManager::manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage, bool isHekaton) {
    int readOnlyCount = (int)(((float)roPercentage / 100.0) * (float)transactionCount);
    int readWriteCount = transactionCount - readOnlyCount;
    vector<Operation> readOnlyOps = Utility::getRandomReadOnlyOps(dataManager, readOnlyCount * dataManager->getOpsPerTransaction());
    vector<Operation> readWriteOps = Utility::getRandomReadWriteOps(dataManager, readWriteCount * dataManager->getOpsPerTransaction());
    vector<future<int>> listenerThreads(transactionCount);
    vector<int> listenerRets(transactionCount);
    _readOnlyOps = &readOnlyOps;
    _readWriteOps = &readWriteOps;

    // Setup read/write left
    setReadOnlyLeft(readOnlyCount);
    setReadWriteLeft(readWriteCount);

    // Create the necessary transactions
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        if(isHekaton) {
            createTransaction();
        }
        else {
            create2PLTransaction();
        }
    }

    // Launch thread futures
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerThreads[i] = async(launch::async, &TransactionManager::transactionListener, this, dataManager, threadCount, isHekaton);
    }

    // Get async promise results
    for(int i = 0; i < readOnlyCount + readWriteCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
    }

    return listenerRets;
}

int TransactionManager::transactionListener(DataManager *dataManager, int threadCount, bool isHekaton) {
    while(true) {
        _concurrentMutex.lock();
        if(_concurrentThreads < threadCount) {
            _concurrentThreads++;
            _concurrentMutex.unlock();
            startTransaction(dataManager, isHekaton);
            _concurrentMutex.lock();
            _concurrentThreads--;
            _concurrentMutex.unlock();
            return 1;
        }
        _concurrentMutex.unlock();
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void TransactionManager::startTransaction(DataManager *db, bool isHekaton) {
	int id = _currId;
	Transaction *t = _transactions[id];
    if(isHekaton) {
        runTransaction(db, t->getReads(), t->getWrites(), t->getIsReadOnly(), t->getId());
    }
    else {
        run2PLTransaction(db, t);
    }
	_currId++;
    
}

void TransactionManager::runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly, int i) {
//     int id = createId();
//     _transactions[id] = new Transaction();
//	cout << "In runTransaction :"<< this_thread::get_id() << endl;
	bool notAborted = true;
    Hekaton *h = new Hekaton();
    h->setId(i);
    h->beginTransaction(db, &_transactions);
    h->read(db, &reads, &_transactions);
    if (!readOnly) {
        notAborted = h->write(db, &writes, &_transactions);
    }
    else {
    //	cout << "BAck in runtransaciton!\n" << endl;
    }
    if (notAborted) {
    	h->endNormalProcessing(db, &_transactions, readOnly);
    }
//    cout << "End runTransaction :"<< this_thread::get_id() << endl;
}

void TransactionManager::run2PLTransaction(DataManager *db, Transaction *t) {
	LockManager *l = new LockManager();
	Lock *lock;
	Lock *nextLock;
	bool aborted = false;
	lock = l->lock(db, t->getTransaction().front().getKey(), t->getId(), t->getTransaction().front().getMode());
	if (lock == NULL) {
			aborted = true;
	}
	if (t->getTransaction().front().getMode() == Operation::Mode::WRITE && !aborted) {
		db->put(t->getTransaction().front().getKey(), t->getTransaction().front().getValue());
	}
	else if (t->getTransaction().front().getMode() == Operation::Mode::READ && !aborted){
		db->get(t->getTransaction().front().getKey());
	}
	int key; 
	vector<Operation> transaction = t->getTransaction();
	for (int i = 1; i < transaction.size() && !aborted; i++) {
		key = transaction[i].getKey();
		nextLock = l->lock(db, key, t->getId(), transaction[i].getMode());
		if (nextLock == NULL) {
			aborted = true;
			break;
		}
		lock->setNextLock(nextLock);
		lock = nextLock;
		if (transaction[i].getMode() == Operation::Mode::WRITE) {
			db->put(key, transaction[i].getValue());
		}
		else {
			db->get(key);
		}
	}
		
// 	for (vector<Operation>::iterator it = next(t->getTransaction().begin()) ; it != t->getTransaction().end(); ++it) {
// 		key = it->getKey();
// 		nextLock = l->lock(key, t->getId(), it->getMode());
// 		if (nextLock == NULL) {
// 			aborted = true;
// 			break;
// 		}
// 		lock->setNextLock(nextLock);
// 		lock = nextLock;
// 		if (it->getMode() == Operation::Mode::WRITE) {
// 			db->put(it->getKey(), it->getValue());
// 		}
// 		else {
// 			db->get(key);
// 			//printf("get: %d\n",db->get(key));
// 		}
// 	}
//	if (!aborted) {
		l->unlock(t->getTransaction().front().getKey(), t->getId());
//	}
} 

Transaction * TransactionManager::createTransaction() {
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
                    reads.push_back(_readOnlyOps->begin()->getKey());
                    _readOnlyOps->erase(_readOnlyOps->begin());
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
//                     if ((int)distribution2(gen)) {
//                         if (readCount !=2) {
//                             reads.push_back(_readWriteOps->begin()->getKey());
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             readCount++;
//                         }
//                         else {
// 			  				writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             writeCount++;
//                         }
//                     }
//                     else  {
//                         if (writeCount !=2) {
// 			  				writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             writeCount++;
//                         }
//                         else {
//                             reads.push_back(_readWriteOps->begin()->getKey());
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             readCount++;
//                         }
//                     }
                    if (_readWriteOps->begin()->getMode() == Operation::Mode::WRITE) {
                    	writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
                        _readWriteOps->erase(_readWriteOps->begin());
                    }
                    else {
                    	reads.push_back(_readWriteOps->begin()->getKey());
                        _readWriteOps->erase(_readWriteOps->begin());
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
//                     if ((int)distribution2(gen)) {
//                         if (readCount !=2) {
//                             reads.push_back(_readWriteOps->begin()->getKey());
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             readCount++;
//                         }
//                         else {
// 			  				writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             writeCount++;
//                         }
//                     }
//                     else  {
//                         if (writeCount !=2) {
// 			  				writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             writeCount++;
//                         }
//                         else {
//                             reads.push_back(_readWriteOps->begin()->getKey());
//                             _readWriteOps->erase(_readWriteOps->begin());
//                             readCount++;
//                         }
//                     }
                    if (_readWriteOps->begin()->getMode() == Operation::Mode::WRITE) {
                    	writes.push_back(make_pair(_readWriteOps->begin()->getKey(), _readWriteOps->begin()->getValue()));
                        _readWriteOps->erase(_readWriteOps->begin());
                    }
                    else {
                    	reads.push_back(_readWriteOps->begin()->getKey());
                        _readWriteOps->erase(_readWriteOps->begin());
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
                    reads.push_back(_readOnlyOps->begin()->getKey());
                    _readOnlyOps->erase(_readOnlyOps->begin());
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


Transaction * TransactionManager::create2PLTransaction() {
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
                    t->addOperation(_readOnlyOps->front());
                    _readOnlyOps->erase(_readOnlyOps->begin());
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
                            t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            readCount++;
                        }
                        else {
			  				t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            writeCount++;
                        }
                        else {
                            t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
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
                            t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            readCount++;
                        }
                        else {
			  				t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
                            writeCount++;
                        }
                        else {
                            t->addOperation(_readWriteOps->front());
                            _readWriteOps->erase(_readWriteOps->begin());
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
                     t->addOperation(_readOnlyOps->front());
                    _readOnlyOps->erase(_readOnlyOps->begin());
                }
                _readOnlyLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        return t;
	}
