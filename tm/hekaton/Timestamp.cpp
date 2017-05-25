#include "Timestamp.h"

Timestamp::Timestamp()
{
    _counter = 0;
    _transactionId = 0;
    _isCounter = true;
}

Timestamp::Timestamp(int counter, int transactionId, bool isCounter)
{
    _counter = counter;
    _transactionId = transactionId;
    _isCounter = isCounter;
}

int Timestamp::getCounter() {
  return _counter;
}
int Timestamp::getTransactionId() {
  return _transactionId;
}
bool Timestamp::isCounter() {
  return _isCounter;
}

void Timestamp::setCounter(int c) {
  _counter = c;
}
void Timestamp::setIsCounter(bool b) {
  _isCounter = b;
}
