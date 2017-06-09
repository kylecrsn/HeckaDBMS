#include "LockEntry.h"

LockEntry::LockEntry() {
	_locks = vector<Lock *>();
	_lockRequests = queue<Lock *>();
}

vector<Lock *> LockEntry::getLocks() {
	return _locks;
}	
queue<Lock *> LockEntry::getLockRequests() {
	return _lockRequests;
}	
void LockEntry::addLockRequest(Lock *lock) {
	_lockRequests.push(lock);
}

bool LockEntry::deleteLock(int objectKey, int transactionId) {
// 	for (vector<Lock *>::iterator it = _locks.begin() ; it != _locks.end(); ++it) {
// 		if ((*it)->getTransactionId() == transactionId) {
// 			_locks.erase(it);
// 			break;
// 		}
// 	}
	for (int i = 0; i < _locks.size(); i++) {
		if (_locks[i]->getTransactionId() == transactionId) {
			_locks.erase(_locks.begin() + i);
			break;
		}
	}
	if (_locks.empty() && _lockRequests.empty()) {
		//delete entry
		return true;
	}
	else {
		tryAddingLocks();
		return false;
	}
}

void LockEntry::tryAddingLocks() {
	_entryMtx.lock();
	bool canAddLocks = true;
	if (_lockRequests.size() != 0) {
		Lock *front = _lockRequests.front();
		Operation::Mode mode = front->getMode();
		int transactionId = front->getTransactionId();
	// 	for (vector<Lock *>::iterator it = _locks.begin() ; it != _locks.end(); ++it) {
	// 		if (mode == Operation::Mode::WRITE && (*it)->getTransactionId() != transactionId) {
	// 			canAddLocks = false;
	// 			break;
	// 		}
	// 	}
		for (int i = 0; i < _locks.size(); i++) {
			if (mode == Operation::Mode::WRITE && _locks[i]->getTransactionId() != transactionId) {
				canAddLocks = false;
				break;
			}
		}
		for (vector<Lock *>::iterator it = _locks.begin() ; it != _locks.end(); ++it) {
			if (mode == Operation::Mode::READ && (*it)->getMode() == Operation::Mode::WRITE && (*it)->getTransactionId() != transactionId) {
				canAddLocks = false;
				break;
			}
		}
		if (canAddLocks) {
			Lock *l;
			bool adding = true;
			while (adding && !_lockRequests.empty()) {
				front = _lockRequests.front();
				mode = front->getMode();
				transactionId = front->getTransactionId();
				for (vector<Lock *>::iterator it = _locks.begin() ; it != _locks.end(); ++it) {
					if (mode == Operation::Mode::WRITE && (*it)->getTransactionId() != transactionId || 
						mode == Operation::Mode::READ && (*it)->getMode() == Operation::Mode::WRITE && (*it)->getTransactionId() != transactionId) {
						adding = false;
						break;
					}
				}
				if (adding) {
					l = _lockRequests.front();
					_lockRequests.pop();
					_locks.push_back(l);
					l->setIsSet(true);
				}
			}
		}
	}
	_entryMtx.unlock();
}
