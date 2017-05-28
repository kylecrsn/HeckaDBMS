#ifndef HECKADBMS_TRANSACTION_H
#define HECKADBMS_TRANSACTION_H

#include "Timestamp.h"
#include <vector>

using namespace std;

class Transaction
{
public:
    // Default Constructor
    Transaction();

    // Destructor
    ~Transaction();

    bool abortNow();
    int getCommitDepCounter();
    int getState();
    Timestamp * getEnd();
    Timestamp * getBegin();
    vector<int> * getCommitDepSet();

    void setState(int s);
    void setCommitDepCounter(int c);
    void setCommitDepSet(vector<int> *c);
    void setAbortNow(bool b);
    
private:
    bool _abortNow;
    int _commitDepCounter;
    int _state; //0 = none, 1 = active, 2 = preparing, 3 = commit, 4 = abort
    Timestamp *_end;
    Timestamp *_begin;
    vector<int> *_commitDepSet;
};
#endif
