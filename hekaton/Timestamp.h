#ifndef HECKADBMS_TIMESTAMP_H
#define HECKADBMS_TIMESTAMP_H

class Timestamp
{
public:
    // Default Constructor
    Timestamp();

    // Destructor
    ~Timestamp();

private:
    int _counter;
    int _transactionId;
    bool _isCounter;
};

#endif