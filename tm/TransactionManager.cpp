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
    float m, c, a, b;
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

    // y is the number of concurrent threads, x is the number of completed transactions
            // y = mx + c
            m = ((float)finalThreadCount - (float)initialThreadCount) / (float)transactionCount;
            c = (float)finalThreadCount;
            break;
        }
        case Utility::EXPONENTIAL: {
            // y = a(b^x)
            a = (float)initialThreadCount;
            b = ((float)finalThreadCount / (float)initialThreadCount);
            break;
        }
    }

    // Launch thread futures
    while(transactionsCompleted < transactionCount) {
        for(int i = 0; i < functionalConcurrentThreads; i++) {
            listenerThreads[i] = async(&TransactionManager::transactionListener, this, dataManager, functionalConcurrentThreads, readOnlyOps, readWriteOps);
        }

        for(int i = 0; transactionsCompleted < transactionCount; i++) {

        }
    }



    // Get async promise results
    for(int i = 0; i < transactionCount; i++) {
        listenerRets[i] = listenerThreads[i].get();
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

/* void TransactionManager::run2PLTransaction(DataManager *db, Transaction *t) {
	for (vector<Operation *>::iterator it = t->getTransaction().begin() ; it != t->getTransaction().end(); ++it) {
		Lock((*it)->getObjectKey(), t->getTransactionId(), (*it)->getMode());
		//do operation
	}
	Unlock(*t->getTransaction().begin(), t->getTransactionId());
} */

Transaction * TransactionManager::createTransaction(vector<Operation> readOnlyOps, vector<Operation> readWriteOps) {
	mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution(0,1000000);
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
                    reads.push_back(*(readOnlyKeys.begin()));
                    readOnlyKeys.erase(readOnlyKeys.begin());
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
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                            readCount++;
                        }
                        else {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                            writeCount++;
                        }
                        else {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
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
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
                            readCount++;
                        }
                        else {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                            writeCount++;
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				writes.push_back(make_pair(readWriteKeys.at(0),(int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                            writeCount++;
                        }
                        else {
                            reads.push_back(*(readWriteKeys.begin()));
                            readWriteKeys.erase(readWriteKeys.begin());
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
                    reads.push_back(*(readOnlyKeys.begin()));
                    readOnlyKeys.erase(readOnlyKeys.begin());
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
  /*	mt19937 gen;
    gen.seed(random_device()());
    uniform_real_distribution<> distribution(0,1000000);
    uniform_real_distribution<> distribution2(0,1);
    int j;
    int readCount;
    int writeCount;
    int w;
    int id;
    Transaction *t = new Transaction();
	if ((int)distribution2(gen)) {
            if (_readOnlyLeft != 0) {
                for (j = 0; j < 4; j++) {
                    t->addOperation(new Operation(Operation::OpMode::READ, *(readOnlyKeys.begin())));
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                _readOnlyLeft--;
                id = createId();
                t->setId(id);
                _transactions[id] = t;
            }
            else {
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            t->addOperation(new Operation(Operation::OpMode::READ, *(readWriteKeys.begin())));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
			  				t->addOperation(new Operation(Operation::OpMode::WRITE, *(readWriteKeys.begin()), (int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
			  				t->addOperation(new Operation(Operation::OpMode::WRITE, *(readWriteKeys.begin()), (int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            t->addOperation(new Operation(Operation::OpMode::READ, *(readWriteKeys.begin())));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                _readWriteLeft--;
               	id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        else {
            if (_readWriteLeft != 0) {
                for (j = 0; j < 4; j++) {
                    if ((int)distribution2(gen)) {
                        if (readCount !=2) {
                            t->addOperation(new Operation(Operation::OpMode::READ, *(readWriteKeys.begin())));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
			  				t->addOperation(new Operation(Operation::OpMode::WRITE, *(readWriteKeys.begin()), (int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                    else  {
                        if (writeCount !=2) {
                        	t->addOperation(new Operation(Operation::OpMode::WRITE, *(readWriteKeys.begin()), (int)distribution(gen)));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                        else {
                            t->addOperation(new Operation(Operation::OpMode::READ, *(readWriteKeys.begin())));
                            readWriteKeys.erase(readWriteKeys.begin());
                        }
                    }
                }
                _readWriteLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
            else {
                for (j = 0; j < 4; j++) {
                    t->addOperation(new Operation(Operation::OpMode::READ, *(readOnlyKeys.begin())));
                    readOnlyKeys.erase(readOnlyKeys.begin());
                }
                _readOnlyLeft--;
                id = createId();
                t->setId(id);
            	_transactions[id] = t;
            }
        }
        return t;*/
    }