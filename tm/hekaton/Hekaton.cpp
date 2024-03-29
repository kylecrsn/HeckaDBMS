#include "Hekaton.h"

Hekaton::Hekaton()
{

}

Hekaton::~Hekaton()
{

}

void Hekaton::setId(int i) {
	_id = i;
}


//sets phase to active and acquire begin timestamp
void Hekaton::beginTransaction(DataManager *db, unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->setState(Transaction::HekatonState::ACTIVE);
	transaction->getBegin()->setCounter(db->getLatestCounter());
	transaction->getBegin()->setIsCounter(true);
}

//see section 2.5, scan db for correct RT and add pointer to readset, call registerCommitDep if applicable
void Hekaton::read(DataManager *db, vector<int> *reads, unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction;
	for (vector<int>::iterator it = reads->begin() ; it != reads->end(); ++it) {
		db->get((*it), transactions, _id, &_readSet);
	} 
}


//Write- see section 2.6, add pointers to writeset, may need to call abort (if so set to postprocessing phase)
bool Hekaton::write(DataManager *db, vector<pair<int, int>> *writes, unordered_map<int, Transaction *> *transactions) {
	bool abortTransaction = false;
	for (vector<pair<int, int>>::iterator it = writes->begin() ; it != writes->end(); ++it) {
		if (!(db->put(it->first, it->second, transactions, _id, &_writeSet))) {
			abortTransaction = true;
			break;
		}
	}
	if (abortTransaction) {
		abort(db, transactions);
		return false;
	}
	return true;
}


//endNormalProcessing - acquire end timestamp, switch state to preparing, call validate
void Hekaton::endNormalProcessing(DataManager *db, unordered_map<int, Transaction *> *transactions, bool readOnly) {
	Transaction *transaction = transactions->at(_id);
	transaction->getEnd()->setCounter(db->getLatestCounter());
	transaction->getEnd()->setIsCounter(true);
	transaction->setState(Transaction::HekatonState::PREPARING);
	if (readOnly) {
		//wait for dependencies
		while (transaction->getCommitDepCounter() != 0 && transaction->getAbortNow() == false) { 
		}
		if (transaction->getAbortNow() == true) {
			abort(db, transactions);
		}
		else if (transaction->getCommitDepCounter() == 0) {
			commit(db, transactions);
		}
	}
	else {
		validate(db, transactions);
	}
}

//reread read objects from db and check if versions are the same as pointers in readset, 
//if valid wait for CommitDepCounter = 0 or AbortNow = 1 (infinite loop), switch to postprocessing phase, may need to call abort
void Hekaton::validate(DataManager *db, unordered_map<int, Transaction *> *transactions) {
	bool valid = true;
	Timestamp *currBegin = transactions->at(_id)->getBegin();
	//check to make sure reads are still visible
	for (vector<Record *>::iterator record = _readSet.begin() ; record != _readSet.end(); ++record) {
		if ((*record)->getBegin()->getIsCounter() && (*record)->getEnd()->getIsCounter() && (*record)->getBegin()->getCounter() != -1) {
			if ((*record)->getEnd()->getCounter() == -1) { //end timestamp must be infinity?
				valid = true;	
			}
			else {
				valid = false;
				break;
			}
		}
		else if (!(*record)->getBegin()->getIsCounter()) {
			Transaction *transaction = transactions->at((*record)->getBegin()->getTransactionId());
			if ((*record)->getBegin()->getTransactionId() == _id && transaction->getState() == 1 && transaction->getEnd()->getIsCounter() && transaction->getEnd()->getCounter() == -1) {
				valid = true;
			}
			else if (transaction->getState() == 2 && transaction->getEnd()->getCounter() < currBegin->getCounter()) {
				valid = true;
			}
			else if (transaction->getState() == 3 && transaction->getEnd()->getCounter() < currBegin->getCounter()) {
				valid = true;
			}
			else {
				valid = false;
				break;
			}
		}
		//end timestamp is transaction ID
		else if (!(*record)->getEnd()->getIsCounter()) {
			Transaction *transaction = transactions->at((*record)->getEnd()->getTransactionId());
			if (transaction->getState() == 1) {
				valid = true;
			}
			else if (transaction->getState() == 2 && transaction->getEnd()->getCounter() > currBegin->getCounter()) {
				valid = true;
			}
			else if (transaction->getState() == 3 && transaction->getEnd()->getCounter() > currBegin->getCounter()) {
				valid = true;
			}
			else if (transaction->getState() == 4) {
				valid = true;
			}
			else {
				valid = false;
				break;
			}
		}
		else {
			valid = false;
			break;
		}
	}
	if (!valid) {
		abort(db, transactions);
		return;
	}
	Transaction *transaction = transactions->at(_id);
	//wait for dependencies
	while (transaction->getCommitDepCounter() != 0 && transaction->getAbortNow() == false) { 
	}
	if (transaction->getAbortNow() == true) {
		abort(db, transactions);
	}
	else if (transaction->getCommitDepCounter() == 0) {
		commit(db, transactions);
	}
}

//change respective timestamps in writeset to inifinity, call abortCommitDep
void Hekaton::abort(DataManager *db, unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->setState(Transaction::HekatonState::ABORTED);
	db->incrementAbortCounter();
	for (vector<Record *>::iterator it = _writeSet.begin() ; it != _writeSet.end(); ++it) {
		db->lock();
		if (!(*it)->getEnd()->getIsCounter() && (*it)->getEnd()->getTransactionId() == _id) {
			(*it)->getEnd()->setIsCounter(true);
			(*it)->getEnd()->setCounter(-1);
			(*it)->setIsLatest(true);
			(*it)->setNextRecord(NULL);
		}
		db->unlock();
		if (!(*it)->getBegin()->getIsCounter()) {
			(*it)->getBegin()->setIsCounter(true);
			(*it)->getBegin()->setCounter(-1);
			(*it)->setIsLatest(false);
		}
	}
	abortCommitDep(transactions);
}

//go through commitDepSet and set respective transactions’ AbortNow flag
void Hekaton::abortCommitDep(unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	for (vector<int>::iterator it = transaction->getCommitDepSet()->begin() ; it != transaction->getCommitDepSet()->end(); ++it) {
	  transactions->at((*it))->setAbortNow(true);
	}
}

//go through writeset and update begin and end timestamps to end timestamp
void Hekaton::commit(DataManager *db, unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->setState(Transaction::HekatonState::COMMITTED);
	db->incrementCommitCounter();
	for (vector<Record *>::iterator it = _writeSet.begin() ; it != _writeSet.end(); ++it) {
		if (!(*it)->getBegin()->getIsCounter()) {
			(*it)->getBegin()->setIsCounter(true);
			(*it)->getBegin()->setCounter(transaction->getEnd()->getCounter());
		}
		if (!(*it)->getEnd()->getIsCounter()) {
			(*it)->getEnd()->setIsCounter(true);
			(*it)->getEnd()->setCounter(transaction->getEnd()->getCounter());
		}
		
	}
	committedCommitDep(transactions);
}

//go through commitDepSet and decrement respective transactions' commitDepCounter
void Hekaton::committedCommitDep(unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	for (vector<int>::iterator it = transaction->getCommitDepSet()->begin() ; it != transaction->getCommitDepSet()->end(); ++it) {
	  transactions->at((*it))->setCommitDepCounter(transactions->at((*it))->getCommitDepCounter()-1);
	}
}
