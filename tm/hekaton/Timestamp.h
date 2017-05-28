#ifndef HECKADBMS_TIMESTAMP_H
#define HECKADBMS_TIMESTAMP_H

class Timestamp
{
public:
    /// Default Constructor
    Timestamp();

    /// Constructor with values for the counter, transaction ID, and which timestamp type it is as parameters
    Timestamp(bool isCounter, int counter, int transactionId);

	Timestamp(const Timestamp &t);
	
    /// Get whether or not the timestamp is a counter or transaction ID
    bool getIsCounter();

    /// Get the counter
    int getCounter();

    /// Get the transaction ID
    int getTransactionId();

    /// Set whether or not the timestamp is a counter or transaction ID
    void setIsCounter(bool isCounter);

    /// Set the counter value
    void setCounter(int counter);

    /// Set the transaction ID
    void setTransactionId(int transactionId);

private:
    bool _isCounter;
    int _counter;
    int _transactionId;
};

#endif
