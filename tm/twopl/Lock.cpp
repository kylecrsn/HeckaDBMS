#include "Lock.h"

Lock::Lock() {
	_objectKey = 0;
	_transactionId = 0;
	_mode = Operation::Mode::WRITE;
	_nextLock = NULL;
}

Lock::Lock(int key, int id, Operation::Mode m, Lock * lock) {
	_objectKey = key;
	_transactionId = id;
	_mode = m;
	_nextLock = lock;
}

Operation::Mode Lock::getMode() {
	return _mode;
}

Lock * Lock::getNextLock() {
	return _nextLock;
}

int Lock::getObjectKey() {
	return _objectKey;
}

int Lock::getTransactionId() {
	return _transactionId;
}

int Lock::getIsSet() {
	return _isSet;
}

void Lock::setIsSet(bool set) {
	_isSet = set;
}

void Lock::setNextLock(Lock *l) {
	_nextLock = l;
}