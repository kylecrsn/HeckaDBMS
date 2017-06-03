#include "Transaction.h"

Transaction::Transaction() {
	_id = 0;
    _abortNow = false;
    _commitDepCounter = 0;
    _commitDepSet = {};
    _state = NONE;
    _end = {};
    _begin = {};
}

Transaction::Transaction(vector<int> reads, vector<pair<int,int>> writes, bool readOnly) {
	_id = 0;
	_abortNow = false;
    _commitDepCounter = 0;
    _commitDepSet = {};
    _state = NONE;
    _end = {};
    _begin = {};
    _reads = reads;
    _writes = writes;
    _readOnly = readOnly;
}

bool Transaction::getAbortNow() {
    return _abortNow;
}

int Transaction::getCommitDepCounter() {
    return _commitDepCounter;
}

vector<int> *Transaction::getCommitDepSet() {
    return _commitDepSet;
}

Transaction::HekatonState Transaction::getState() {
    return _state;
}

Timestamp *Transaction::getBegin() {
    return _begin;
}

Timestamp *Transaction::getEnd() {
    return _end;
}

vector<int> Transaction::getReads() {
	return _reads;
}
    
vector<pair<int,int>> Transaction::getWrites() {
	return _writes;
}
    
bool Transaction::getIsReadOnly() {
	return _readOnly;
}

int Transaction::getId() {
	return _id;
}

vector<Operation *> Transaction::getTransaction() {
	return _transaction;
}

void Transaction::setAbortNow(bool abortNow) {
    _abortNow = abortNow;
}

void Transaction::setCommitDepCounter(int commitDepCounter) {
    _commitDepCounter = commitDepCounter;
}

void Transaction::setCommitDepSet(vector<int> *commitDepSet) {
    _commitDepSet = commitDepSet;
}

void Transaction::setState(Transaction::HekatonState state) {
    _state = state;
}

void Transaction::setBegin(Timestamp *begin) {
    _begin = begin;
}

void Transaction::setEnd(Timestamp *end) {
    _end = end;
}

void Transaction::addOperation(Operation *o) {
	_transaction.push_back(o);
}

void Transaction::setId(int i) {
	_id = i;
}
