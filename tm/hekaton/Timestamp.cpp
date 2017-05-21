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

Timestamp::~Timestamp()
{ }