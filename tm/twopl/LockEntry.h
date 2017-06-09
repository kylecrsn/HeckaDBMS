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
    
    /// Gets the locks that are set for an object
	vector<Lock *> getLocks();
	
	/// Gets the lock requests for an object
	queue<Lock *> getLockRequests();
	
	/// Adds a lock request to the lock requests queue for the object
	void addLockRequest(Lock *lock);
	
	/// Deletes a lock from the lock vector
	bool deleteLock(int key, int transactionId);
	
	/// Tries to set locks on an object from the lock requests queue
	void tryAddingLocks();

private:
	mutex _entryMtx;
    vector<Lock *> _locks;
    queue<Lock *> _lockRequests;
};

#endif