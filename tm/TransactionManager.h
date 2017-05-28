#ifndef HECKADBMS_TRANSACTIONMANAGER_H
#define HECKADBMS_TRANSACTIONMANAGER_H

#include "Hekaton.h"
#include "DataManager.h"
#include "Transaction.h"
#include <mutex>

class TransactionManager
{
public:
    /// Default Constructor
    TransactionManager();

	createId();

	runTransaction(vector<int> readOnlyKeys, vector<pair<int,int>> readWriteKeys);

private:
	int _idCounter;
    vector<unordered_map<int, Transaction *> *transactions> _transactions;
};

#endif