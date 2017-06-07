#include "LockManager.h"

LockManager::LockManager() {
	_lockTable = {};
}
Lock * LockManager::lock(int objectKey, int transactionId, Operation::Mode mode) {
	if (_lockTable.find(objectKey) == _lockTable.end()) {
		Lock *lock = new Lock(objectKey, transactionId, mode, NULL);
		LockEntry *entry = new LockEntry();
		entry->addLockRequest(lock);
		bool added = false;
		_tableMtx.lock();
		if (_lockTable.find(objectKey) == _lockTable.end()) {
			_lockTable[objectKey] = entry;
			added = true;
			_lockTable[objectKey]->tryAddingLocks();
		}
		_tableMtx.unlock();
		if (!added) {
			_lockTable[objectKey]->addLockRequest(lock);
			_lockTable[objectKey]->tryAddingLocks();
			bool notSet = true;
			time_t timer = time(NULL);
			while (notSet) {
				if (difftime(timer, time(NULL)) >= 2.0) {
					lock = NULL;
					break;
				}
				if (lock->getIsSet()) {
					notSet = false;
				}
			}
		}
		return lock;		
	}
	else {
		Lock *lock = new Lock(objectKey, transactionId, mode, NULL);
		_lockTable[objectKey]->addLockRequest(lock);
		_lockTable[objectKey]->tryAddingLocks();
		bool notSet = true;
		time_t timer = time(NULL);
		while (notSet) {
			if (difftime(timer, time(NULL)) >= 2.0) {
				lock = NULL;
				break;
			}
			if (lock->getIsSet()) {
				notSet = false;
			}
		}
		return lock;
	}
}
    
void LockManager::unlock(int objectKey, int transactionId) {
	bool deleteEntry = false;
	Lock *lock;
	for (vector<Lock *>::iterator it = _lockTable[objectKey]->getLocks().begin() ; it != _lockTable[objectKey]->getLocks().end(); ++it) {
		if ((*it)->getTransactionId() == transactionId) {
			lock = (*it);
			break;
		}
	}
	Lock *nextLock = lock->getNextLock();
	deleteEntry = _lockTable[objectKey]->deleteLock(objectKey, transactionId);  //deleteLock will call tryAddingLock and delete entry if no more locks or requests
	if (deleteEntry) {
		_tableMtx.lock();
		if (_lockTable[objectKey]->getLocks().size() == 0 && _lockTable[objectKey]->getLockRequests().size() == 0) {
			_lockTable.erase(objectKey);
		}
		_tableMtx.unlock();
	}
	while (nextLock) {
		int key = nextLock->getObjectKey();
		lock = nextLock;
		nextLock = lock->getNextLock();
		deleteEntry = _lockTable[objectKey]->deleteLock(lock->getObjectKey(), transactionId);  //deleteLock will call tryAddingLock and delete entry if no more locks or requests
		if (deleteEntry) {
			_lockTable.erase(objectKey);
		}
	}
	
}
