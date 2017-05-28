#include "Transaction.h"

Transaction::Transaction() {
    _abortNow = false;
    _commitDepCounter = 0;
    _commitDepSet = {};
    _state = NONE;
    _end = {};
    _begin = {};
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

Timestamp * Transaction::getEnd() {
    return _end;
}

Timestamp * Transaction::getBegin() {
	return _begin;
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

void Transaction::setEnd(Timestamp *end) {
    _end = end;
}

void Transaction::setBegin(Timestamp *begin) {
    _begin = begin;
}