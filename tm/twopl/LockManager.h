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
    
    
    Lock * lock(int objectKey, int transactionId, Operation::Mode mode);
    
    void unlock(int objectKey, int transactionId);

private:
	mutex _tableMtx;
    unordered_map<int, LockEntry *>  _lockTable;
};

#endif