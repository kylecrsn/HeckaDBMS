#ifndef HECKADBMS_TRANSACTIONMANAGER_H
#define HECKADBMS_TRANSACTIONMANAGER_H

#include <thread>
#include <random>
#include <tuple>

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

    void runTransaction(DataManager *db, vector<int> reads, vector<pair<int,int>> writes, bool readOnly);

    void createTransactions(DataManager *db, int readOnlyCount, int readWriteCount, vector<int> readOnlyKeys, vector<int> readWriteKeys, int threadCount);

private:
    int _idCounter;
    unordered_map<int, Transaction *>  _transactions;
};

#endif