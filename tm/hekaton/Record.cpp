#include "Record.h"


Record::Record()
{

}

Record::~Record()
{

}

Timestamp Record::getBegin() {
  return _begin;
}
Timestamp Record::getEnd() {
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
