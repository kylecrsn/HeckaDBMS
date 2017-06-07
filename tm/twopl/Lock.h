#ifndef HECKADBMS_LOCK_H
#define HECKADBMS_LOCK_H

#include "Operation.h"
using namespace std;

class Lock
{
public:
    /// Default Constructor
    Lock();
        
    
    Lock(int key, int id, Operation::Mode m, Lock * lock);
    
    Operation::Mode getMode();
    
    Lock * getNextLock();
    
    int getObjectKey();
    
    int getTransactionId();
    
    int getIsSet();
    
    void setIsSet(bool set);
    
    void setNextLock(Lock *l);
    

private:
	int _objectKey;
    int _transactionId;
    int _isSet;
    Operation::Mode _mode;
    Lock *_nextLock;
};

#endif