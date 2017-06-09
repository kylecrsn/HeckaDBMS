#ifndef HECKADBMS_LOCKMANAGER_H
#define HECKADBMS_LOCKMANAGER_H


#include "DataManager.h"
#include "LockEntry.h"
#include "Operation.h"
#include <ctime>




class LockManager
{
public:
    /// Default Constructor
    LockManager();
    
    /// Tries to set a lock for the transaction's operation
    Lock * lock(int key, int transactionId, Operation::Mode mode);
    
    /// Unlocks all of a transaction's operations
    void unlock(int key, int transactionId);

private:
	mutex _tableMtx;
    unordered_map<int, LockEntry *>  _lockTable;
};

#endif