#include "Timestamp.h"

Timestamp::Timestamp() {
    _isCounter = true;
    _counter = 0;
    _transactionId = 0;
}

Timestamp::Timestamp(bool isCounter, int counter, int transactionId) {
    _isCounter = isCounter;
    _counter = counter;
    _transactionId = transactionId;
}

bool Timestamp::getIsCounter() {
    return _isCounter;
}

int Timestamp::getCounter() {
    return _counter;
}

int Timestamp::getTransactionId() {
    return _transactionId;
}

void Timestamp::setIsCounter(bool isCounter) {
    _isCounter = isCounter;
}

void Timestamp::setCounter(int counter) {
    _counter = counter;
}

void Timestamp::setTransactionId(int transactionId) {
    _transactionId = transactionId;
}
