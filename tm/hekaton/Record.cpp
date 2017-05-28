#include "Record.h"


Record::Record()
{

}

Record::Record(Timestamp *tBegin, Timestamp *tEnd, int object, int value) {
	_begin = tBegin;
	_end = tEnd;
	_object = object;
	_value = value;
}

Record::~Record()
{

}

Timestamp * Record::getBegin() {
  return _begin;
}
Timestamp * Record::getEnd() {
  return _end;
}
int Record::getObject() {
  return _object;
}
int Record::getValue() {
  return _value;
}
Record * Record::getNextRecord() {
  return _nextRecord;
}

void Record::setBegin(Timestamp *t) {
	_begin = t;
}
void Record::setEnd(Timestamp *t) {
	_end = t;
}
void Record::setNextRecord(Record *r) {
	_nextRecord = r;
}