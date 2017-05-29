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
/*	  Record *record = db->Get((*it), transactions, &_readSet);
	  while (record) {
			//begin and end timestamps are set
			if (record->getBegin().getgetIsCounter() && record->getEnd().getgetIsCounter() && record->getBegin().getCounter() != -1) {
				if (record->getBegin().getCounter() < _begin.getCounter() && record->getEnd().getCounter() == -1) { //end timestamp must be infinity?
					_readSet.push_back(record);
					break;
				}
				else {
					record = record->getNextRecord();
				}
			}
			//begin timestamp is transaction ID
			else if (!record->getBegin().getgetIsCounter()) {
				transaction = transactions->at(record->getBegin().getTransactionId());
				if (record->getBegin().getTransactionId() == _id && transaction->getState() == 1 && transaction->getEnd().getgetIsCounter() && transaction->getEnd().getCounter() == -1) {
					_readSet.push_back(record);
					break;
				}
				else if (transaction->getState() == 2 && transaction->getEnd().getCounter() < _begin.getCounter()) {
					_readSet.push_back(record);
					registerCommitDep(record->getBegin().getTransactionId(), transactions);
					break;
				}
				else if (transaction->getState() == 3 && transaction->getEnd().getCounter() < _begin.getCounter()) {
					_readSet.push_back(record);
					break;
				}
				//not sure what to do about terminated transaction
			}
			//end timestamp is transaction ID
			else if (!record->getEnd().getgetIsCounter()) {
				transaction = transactions->at(record->getEnd().getTransactionId());
				if (transaction->getState() == 2 && transaction->getEnd().getCounter() > _begin.getCounter()) {
					_readSet.push_back(record);
				}
				else if (transaction->getState() == 2 && transaction->getEnd().getCounter() < _begin.getCounter()) {
					registerCommitDep(record->getEnd().getTransactionId(), transactions);
					record = record->getNextRecord(); //speculatively ignore?
				}
				else if (transaction->getState() == 3 && transaction->getEnd().getCounter() > _begin.getCounter()) {
					_readSet.push_back(record);
					break;
				}
				else if (transaction->getState() == 4) {
					_readSet.push_back(record);
					break;
				}
				//not sure what to do about terminated transaction
			}
		} */
	} 
}


//registerCommitDep(T1, T2) - increment T2’s commitDepCounter and update T1’s commitDepSet with T2
void Hekaton::registerCommitDep(int transaction, unordered_map<int, Transaction *> *transactions) {
	Transaction *firstTransaction = transactions->at(transaction);
	Transaction *secondTransaction = transactions->at(_id);
	secondTransaction->setCommitDepCounter(secondTransaction->getCommitDepCounter()+1);
	firstTransaction->getCommitDepSet()->push_back(_id);
}

//Write- see section 2.6, add pointers to writeset, may need to call abort (if so set to postprocessing phase)
void Hekaton::write(DataManager *db, vector<pair<int, int>> *writes, unordered_map<int, Transaction *> *transactions) {
	bool abortTransaction = false;
	for (vector<pair<int, int>>::iterator it = writes->begin() ; it != writes->end(); ++it) {
		if (!(db->put(it->first, it->second, transactions, _id, &_writeSet))) {
			abortTransaction = true;
			break;
		}
	}
	if (abortTransaction) {
		abort(transactions);
	}
	else {
		endNormalProcessing(db, transactions);
	}
}


//endNormalProcessing - acquire end timestamp, switch state to preparing, call validate
void Hekaton::endNormalProcessing(DataManager *db, unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->getEnd()->setCounter(db->getLatestCounter());
	transaction->getEnd()->setIsCounter(true);
	transaction->setState(Transaction::HekatonState::PREPARING);
	validate(transactions);
}

//reread read objects from db and check if versions are the same as pointers in readset, 
//if valid wait for CommitDepCounter = 0 or AbortNow = 1 (infinite loop), switch to postprocessing phase, may need to call abort
void Hekaton::validate(unordered_map<int, Transaction *> *transactions) {
	bool valid = false;
	Timestamp *currBegin = transactions->at(_id)->getBegin();
	//check to make sure reads are still visible
	for (vector<Record *>::iterator record = _readSet.begin() ; record != _readSet.end(); ++record) {
		if ((*record)->getBegin()->getIsCounter() && (*record)->getEnd()->getIsCounter() && (*record)->getBegin()->getCounter() != -1) {
			if ((*record)->getBegin()->getCounter() < currBegin->getCounter() && (*record)->getEnd()->getCounter() == -1) { //end timestamp must be infinity?
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
				valid = true;;
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
			Transaction *transaction = transactions->at((*record)->getBegin()->getTransactionId());
			if (transaction->getState() == 2 && transaction->getEnd()->getCounter() > currBegin->getCounter()) {
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
	}
	if (!valid) {
		abort(transactions);
	}
	Transaction *transaction = transactions->at(_id);
	//wait for dependencies
	while (transaction->getCommitDepCounter() != 0 || transaction->getAbortNow() == false) {
	}
	if (transaction->getAbortNow() == true) {
		abort(transactions);
	}
	else if (transaction->getCommitDepCounter() == 0) {
		commit(transactions);
	}
}

//change respective timestamps in writeset to inifinity, call abortCommitDep
void Hekaton::abort(unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->setState(Transaction::HekatonState::ABORTED);
	for (vector<Record *>::iterator it = _writeSet.begin() ; it != _writeSet.end(); ++it) {
		if (!(*it)->getBegin()->getIsCounter()) {
			(*it)->getBegin()->setIsCounter(true);
			(*it)->getBegin()->setCounter(-1);
		}
		if (!(*it)->getEnd()->getIsCounter()) {
			(*it)->getEnd()->setIsCounter(true);
			(*it)->getEnd()->setCounter(-1);
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
void Hekaton::commit(unordered_map<int, Transaction *> *transactions) {
	Transaction *transaction = transactions->at(_id);
	transaction->setState(Transaction::HekatonState::COMMITTED);
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
