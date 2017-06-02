#ifndef HECKADBMS_TRANSACTIONMANAGER_H
#define HECKADBMS_TRANSACTIONMANAGER_H

#include <thread>
#include <random>

#include "Utility.h"
#include "Hekaton.h"
#include "DataManager.h"
#include "Transaction.h"

using namespace std;

class TransactionManager
{
public:
    /// Default Constructor
    TransactionManager();

    int createId();

    void manageManualTransactions(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys);
    void manualListener(DataManager *dataManager, int threadCount, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys);
    void manageScaleTransactions(DataManager *dataManager, int transactionCount, int initialThreadCount, int finalThreadCount, Utility::ScaleAlgorithm scaleAlgorithm);
    void manageVaryTransactions(DataManager *dataManager, int transactionCount, int threadCount, int roPercentage);

    void runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly);
    
    Transaction * createTransactions(int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys);

    void createThreads(DataManager *db, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys, int threadCount);

private:
    int _concurrentThreads;
    boost::atomic<int> _idCounter;
    unordered_map<int, Transaction *>  _transactions;
};

#endif
