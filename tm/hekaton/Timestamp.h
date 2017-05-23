#ifndef HECKADBMS_TIMESTAMP_H
#define HECKADBMS_TIMESTAMP_H

class Timestamp
{
public:
    // Default Constructor
    Timestamp();

    // Constructor with values for the counter, transaction ID, and which timestamp type it is as parameters
    Timestamp(int counter, int transactionId, bool isCounter);

private:
    int _counter;
    int _transactionId;
    bool _isCounter;
};

#endif