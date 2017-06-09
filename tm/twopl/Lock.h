#ifndef HECKADBMS_LOCK_H
#define HECKADBMS_LOCK_H

#include "Operation.h"
using namespace std;

class Lock
{
public:
    /// Default Constructor
    Lock();
        
    /// Constructor taking key, transaction id, and mode
    Lock(int key, int id, Operation::Mode m);
    
    /// Gets the mode of a lock
    Operation::Mode getMode();
    
    /// Gets the next lock of the current lock in a transaction
    Lock * getNextLock();
    
    /// Gets the lock's key
    int getKey();
    
    /// Gets the lock's transaction id
    int getTransactionId();
    
    /// Gets if the lock is set
    int getIsSet();
    
    /// Sets the lock as true or false
    void setIsSet(bool set);
    
    /// Sets the next lock of the current lock in a transaction
    void setNextLock(Lock *l);
    

private:
	int _key;
    int _transactionId;
    int _isSet;
    Operation::Mode _mode;
    Lock *_nextLock;
};

#endif