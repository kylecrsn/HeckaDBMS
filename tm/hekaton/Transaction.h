#ifndef HECKADBMS_TRANSACTION_H
#define HECKADBMS_TRANSACTION_H

#include <vector>

#include "Timestamp.h"

using namespace std;

class Transaction
{
public:
    /// Default Constructor
    Transaction();

    /// Defines the different states the transaction can be in
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

    /// Get the end timestamp of the transaction
    Timestamp getEnd();

    /// Set whether or not to abort now
    void setAbortNow(bool abortNow);

    /// Set the commit dependency counter
    void setCommitDepCounter(int commitDepCounter);

    /// Set the commit dependency set
    void setCommitDepSet(vector<int> *commitDepSet);

    /// Set the current state of the transaction
    void setState(HekatonState state);

    /// Set the end timestamp of the transaction
    void setEnd(Timestamp end);
    
private:
    bool _abortNow;
    int _commitDepCounter;
    vector<int> *_commitDepSet;
    HekatonState _state;
    Timestamp _end;
    //int _state; //0 = none, 1 = active, 2 = preparing, 3 = commit, 4 = abort
};

#endif
