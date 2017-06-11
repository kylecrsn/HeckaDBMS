#include "LockManager.h"

LockManager::LockManager() {
	_lockTable = {};
}
Lock * LockManager::lock(DataManager *db, int key, int transactionId, Operation::Mode mode) {
	if (_lockTable.find(key) == _lockTable.end()) {
		Lock *lock = new Lock(key, transactionId, mode);
		LockEntry *entry = new LockEntry();
		entry->addLockRequest(lock);
		bool added = false;
		_tableMtx.lock();
		if (_lockTable.find(key) == _lockTable.end()) {
			_lockTable[key] = entry;
			added = true;
		}
		_tableMtx.unlock();
		if (!added) {
			_lockTable[key]->addLockRequest(lock);
		}
		_lockTable[key]->tryAddingLocks();
		bool notSet = true;
		time_t timer = time(NULL);
		while (notSet) {
			if (difftime(timer, time(NULL)) >= 2.0) {
	//			cout << "Aborted in 2PL\n";
				db->incrementAbortCounter();
				return NULL;
			}
			if (lock->getIsSet()) {
				notSet = false;
			}
		}
		db->incrementCommitCounter();
		return lock;		
	}
	else {
		Lock *lock = new Lock(key, transactionId, mode);
		_lockTable[key]->addLockRequest(lock);
		_lockTable[key]->tryAddingLocks();
		bool notSet = true;
		time_t timer = time(NULL);
		while (notSet) {
			if (difftime(timer, time(NULL)) >= 2.0) {
	//			cout << "Aborted in else of 2PL\n";
				db->incrementAbortCounter();
				return NULL;
			}
			if (lock->getIsSet()) {
				notSet = false;
			}
		}
		db->incrementCommitCounter();
		return lock;
	}
}
    
void LockManager::unlock(int key, int transactionId) {
	bool deleteEntry = false;
	Lock *lock;
	vector<Lock *> locks = _lockTable[key]->getLocks();
// 	for (int i = 0; i < locks.size(); i++) {
// 		if (locks[i]->getTransactionId() == transactionId) {
// 			lock = locks[i];
// 			break;
// 		}
// 	}
	for (vector<Lock *>::iterator it = locks.begin(); it != locks.end(); ++it) {
		if ((*it)->getTransactionId() == transactionId) {
			lock = (*it);
			break;
		}
	}
	Lock *nextLock = lock->getNextLock();
	deleteEntry = _lockTable[key]->deleteLock(key, transactionId);  //deleteLock will call tryAddingLock and delete entry if no more locks or requests
	if (deleteEntry) {
		_tableMtx.lock();
		if (_lockTable[key]->getLocks().size() == 0 && _lockTable[key]->getLockRequests().size() == 0) {
			_lockTable.erase(key);
		}
		_tableMtx.unlock();
	}
	int entryKey;
	while (nextLock) {
		lock = nextLock;
		entryKey = lock->getKey();
		nextLock = lock->getNextLock();
		deleteEntry = _lockTable[entryKey]->deleteLock(entryKey, transactionId);  //deleteLock will call tryAddingLock and delete entry if no more locks or requests
		if (deleteEntry) {
			_tableMtx.lock();
			if (_lockTable[entryKey]->getLocks().size() == 0 && _lockTable[entryKey]->getLockRequests().size() == 0) {
				_lockTable.erase(entryKey);
			}
			_tableMtx.unlock();
		}
	}	
}
