#ifndef HECKADBMS_TRANSACTIONMANAGER_H
#define HECKADBMS_TRANSACTIONMANAGER_H

#include <thread>
#include <random>

#include "Utility.h"
#include "Hekaton.h"
#include "DataManager.h"
#include "Transaction.h"
#include "Operation.h"

using namespace std;

class TransactionManager
{
public:
    /// Default Constructor
    TransactionManager();
    
    void setReadOnlyLeft(int l);
    
    void setReadWriteLeft(int l);

    int createId();

    void manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys);
    void manualListener(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys);
    void manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, Utility::ScaleAlgorithm scaleAlgorithm);
    void manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage);

    void runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly, int i);
    
    Transaction * createTransaction(vector<int> readOnlyKeys, vector<int> readWriteKeys);

	Transaction * create2PLTransaction(vector<int> readOnlyKeys, vector<int> readWriteKeys);

    void startTransaction(DataManager *db, vector<int> readOnlyKeys, vector<int> readWriteKeys);

private:
    int _concurrentThreads;
    boost::atomic<int> _idCounter;
    mutex _transactionMtx;
    unordered_map<int, Transaction *>  _transactions;
    int _readOnlyLeft;
    int _readWriteLeft;
};

#endif
