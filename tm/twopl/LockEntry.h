#ifndef HECKADBMS_LOCKENTRY_H
#define HECKADBMS_LOCKENTRY_H

#include "Lock.h"
#include <mutex>
#include <queue>

using namespace std;

class LockEntry
{
public:
    /// Default Constructor
    LockEntry();
    
	vector<Lock *> getLocks();
	
	queue<Lock *> getLockRequests();
	
	void addLockRequest(Lock *lock);
	
	bool deleteLock(int objectKey, int transactionId);
	
	void tryAddingLocks();

private:
	mutex _entryMtx;
    vector<Lock *> _locks;
    queue<Lock *> _lockRequests;
};

#endif