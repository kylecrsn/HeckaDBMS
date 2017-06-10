#ifndef HECKADBMS_TRANSACTIONMANAGER_H
#define HECKADBMS_TRANSACTIONMANAGER_H

#include <thread>
#include <future>
#include <random>
#include <pthread.h>

#include "Utility.h"
#include "Hekaton.h"
#include "DataManager.h"
#include "Transaction.h"
#include "Operation.h"
#include "LockManager.h"

using namespace std;

class TransactionManager
{
public:
    /// Default Constructor
    TransactionManager();

    /// Manage read-only transaction count
    void setReadOnlyLeft(int l);

    /// Manage read-write transaction count
    void setReadWriteLeft(int l);

    /// Create a new unique transaction ID
    int createId();

    /// Entry point from manual test client into TM, spawns and joins listener threads
    vector<int> manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<Operation> readOnlyKeys, vector<Operation> readWriteKeys, bool isHekaton);

    /// Entry point from scale test client into TM, spawns and joins listener threads
    vector<int> manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, bool isHekaton);

    /// Entry point from vary test client into TM, spawns and joins listener threads
    vector<int> manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage, bool isHekaton);

    /// Listener, simply attempts to spawn a transaction thread
    int transactionListener(DataManager *dataManager, int threadCount, bool isHekaton);

    /// Creates and runs a Hekaton transaction, spawned by listener
    void startTransaction(DataManager *db, bool isHekaton);

    /// Creates a Hekaton object for a transaction
    Transaction * createTransaction();
    
    // Executes a transaction with a 2PL object
    void run2PLTransaction(DataManager *db, Transaction *t);

    /// Executes a transaction with a Hekaton object
    void runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly, int i);

    /// Creates a 2PL object for a transaction
    Transaction * create2PLTransaction();
private:
    mutex _concurrentMutex;
    int _concurrentThreads;
    boost::atomic<int> _idCounter;
    boost::atomic<int> _currId;
    mutex _transactionMtx;
    unordered_map<int, Transaction *>  _transactions;
    int _readOnlyLeft;
    int _readWriteLeft;
    vector<Operation> *_readOnlyOps;
    vector<Operation> *_readWriteOps;
};

#endif
