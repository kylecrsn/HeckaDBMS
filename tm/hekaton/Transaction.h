#ifndef HECKADBMS_TRANSACTION_H
#define HECKADBMS_TRANSACTION_H

#include <vector>

#include "Timestamp.h"
#include "Operation.h"

using namespace std;

class Transaction
{
public:
    /// Default Constructor
    Transaction();
    
    Transaction(vector<int> reads, vector<pair<int,int>> writes, bool readOnly);

    /// Defines the different states the transaction can be in
    /// Old state reference: 0 = none, 1 = active, 2 = preparing, 3 = commit, 4 = abort
    enum HekatonState {
        NONE,
        ACTIVE,
        PREPARING,
        ABORTED,
        COMMITTED
    };

    /// Get whether or not to abort now
    bool getAbortNow();

    /// Get the commit dependency counter
    int getCommitDepCounter();

    /// Get the commit dependency set
    vector<int> *getCommitDepSet();

    /// Get the current state of the transaction
    HekatonState getState();

    /// Get the begin timestamp of the transaction
    Timestamp *getBegin();

    /// Get the end timestamp of the transaction
    Timestamp *getEnd();
    
    /// Get the read objects of the transaction
    vector<int> getReads();
    
    ///Get the write objects of the transaction
    vector<pair<int,int>> getWrites();
    
    ///Get whether transaction is read only
    bool getIsReadOnly();
    
    int getId();
    
    vector<Operation *> getTransaction();

    /// Set whether or not to abort now
    void setAbortNow(bool abortNow);

    /// Set the commit dependency counter
    void setCommitDepCounter(int commitDepCounter);

    /// Set the commit dependency set
    void setCommitDepSet(vector<int> *commitDepSet);

    /// Set the current state of the transaction
    void setState(HekatonState state);

    /// Set the begin timestamp of the transaction
    void setBegin(Timestamp *begin);

    /// Set the end timestamp of the transaction
    void setEnd(Timestamp *end);
    
    void setId(int i);
    
    void addOperation(Operation *o);
    
private:
	int _id;
    bool _abortNow;
    int _commitDepCounter;
    vector<int> *_commitDepSet;
    HekatonState _state;
    Timestamp *_end;
    Timestamp *_begin;
    vector<int> _reads;
    vector<pair<int,int>> _writes;
    vector<Operation *> _transaction;
    bool _readOnly;
};

#endif
