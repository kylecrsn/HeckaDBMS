#include "Record.h"

Record::Record() {
    _entryKey = 0;
    _isLatest = true;
    _objectKey = 0;
    _objectValue = 0;
    _begin = new Timestamp{};
    _end = new Timestamp{};
    _end->setCounter(-1);
    _nextRecord = nullptr;
}

Record::Record(int entryKey, int objectKey, int objectValue) {
    _entryKey = entryKey;
    _isLatest = true;
    _objectKey = objectKey;
    _objectValue = objectValue;
    _begin = new Timestamp{};
    _end = new Timestamp{};
    _end->setCounter(-1);
    _nextRecord = nullptr;
}

Record::Record(Timestamp *begin, Timestamp *end, int entryKey, int objectKey, int objectValue) {
	_entryKey = entryKey;
    _isLatest = true;
    _objectKey = objectKey;
    _objectValue = objectValue;
    _begin = begin;
    _end = end;
    _nextRecord = nullptr;
}

int Record::getEntryKey() {
    return _entryKey;
}

bool Record::getIsLatest() {
    return _isLatest;
}

int Record::getObjectKey() {
    return _objectKey;
}

int Record::getObjectValue() {
    return _objectValue;
}

Timestamp * Record::getBegin() {
    return _begin;
}

Timestamp * Record::getEnd() {
    return _end;
}

Record* Record::getNextRecord() {
    return _nextRecord;
}

void Record::setEntryKey(int entryKey) {
    _entryKey = entryKey;
}

void Record::setIsLatest(bool isLatest) {
    _isLatest = isLatest;
}

void Record::setObjectKey(int objectKey) {
    _objectKey = objectKey;
}

void Record::setObjectValue(int objectValue) {
    _objectValue = objectValue;
}

void Record::setBegin(Timestamp *begin) {
    _begin = begin;
}

void Record::setEnd(Timestamp *end) {
    _end = end;
}

void Record::setNextRecord(Record *nextRecord) {
    _nextRecord = nextRecord;
}