#ifndef HEKADBMS_HEKATON_H
#define HEKADBMS_HEKATON_H

#include <vector>

#include "Record.h"
#include "Timestamp.h"
#include "DataManager.h"
#include "Transaction.h"

using namespace std;

class Hekaton
{
public:
    /// Default Constructor
    Hekaton();

    /// Destructor
    ~Hekaton();
    
	//sets phase to active and acquire begin timestamp
    void beginTransaction(DataManager *db, unordered_map<int, Transaction *> *transactions);
	
	//see section 2.5, scan db for correct RT and add pointer to readset, call registerCommitDep if applicable
	void read(DataManager *db, vector<int> *reads, unordered_map<int, Transaction *> *transactions);
	
	//registerCommitDep(T1, T2) - increment T2’s commitDepCounter and update T1’s commitDepSet with T2
	void registerCommitDep(int transaction, unordered_map<int, Transaction *> *transactions);

	//Write- see section 2.6, add pointers to writeset, may need to call abort (if so set to postprocessing phase)
	void write(DataManager *db, vector<pair<int, int>> *writes, unordered_map<int, Transaction *> *transactions);
	
	//endNormalProcessing - acquire end timestamp, switch state to preparing, call validate
	void endNormalProcessing(DataManager *db, unordered_map<int, Transaction *> *transactions);
	
	//reread read objects from db and check if versions are the same as pointers in readset, 
	//if valid wait for CommitDepCounter = 0 or AbortNow = 1 (infinite loop), switch to postprocessing phase, may need to call abort
	void validate(unordered_map<int, Transaction *> *transactions);
	
	//change respective timestamps in writeset to inifinity, call abortCommitDep
	void abort(unordered_map<int, Transaction *> *transactions);

	//go through commitDepSet and set respective transactions’ AbortNow flag
	void abortCommitDep(unordered_map<int, Transaction *> *transactions);
	
	//go through writeset and update begin and end timestamps to end timestamp
	void commit(unordered_map<int, Transaction *> *transactions);
	
	//go through commitDepSet and decrement respective transactions' commitDepCounter
	void committedCommitDep(unordered_map<int, Transaction *> *transactions);

 private:
    int _id;
    vector<Record *> _readSet;
    vector<Record *> _writeSet;
};

#endif
