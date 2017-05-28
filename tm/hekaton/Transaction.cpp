
#include "Transaction.h"

Transaction::Transaction()
{
  _abortNow = false;
  _commitDepCounter = 0;
  _state = 0;
  Timestamp *_end = new Timestamp();
  Timestamp *_begin = new Timestamp();
  vector<int> *_commitDepSet;
}

Transaction::~Transaction()
{

}

bool Transaction::abortNow() {
  return _abortNow;
}
int Transaction::getCommitDepCounter() {
  return _commitDepCounter;
}
int Transaction::getState() {
  return _state;
}
Timestamp * Transaction::getEnd() {
  return _end;
}
Timestamp * Transaction::getBegin() {
	return _begin;
}
vector<int>* Transaction::getCommitDepSet() {
  return _commitDepSet;
}

void Transaction::setState(int s) {
  _state = s;
}
void Transaction::setCommitDepCounter(int c) {
  _commitDepCounter = c;
}
void Transaction::setCommitDepSet(vector<int> * c) {
  _commitDepSet = c;
}
void Transaction::setAbortNow(bool b) {
  _abortNow = b;
}
