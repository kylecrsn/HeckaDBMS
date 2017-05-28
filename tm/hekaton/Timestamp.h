#ifndef HECKADBMS_TIMESTAMP_H
#define HECKADBMS_TIMESTAMP_H

class Timestamp
{
public:
    // Default Constructor
    Timestamp();

    // Constructor with values for the counter, transaction ID, and which timestamp type it is as parameters
    Timestamp(int counter, int transactionId, bool isCounter);

	Timestamp(const Timestamp &t);
	
    int getCounter();
    int getTransactionId();
    bool isCounter();

    void setCounter(int c);
    void setIsCounter(bool b);
    void setTransactionId(int i);
private:
    int _counter;
    int _transactionId;
    bool _isCounter;
};

#endif
