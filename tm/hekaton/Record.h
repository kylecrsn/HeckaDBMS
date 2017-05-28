#ifndef HECKADBMS_RECORD_H
#define HECKADBMS_RECORD_H

#include "Timestamp.h"

class Record
{
public:
    // Default constructor
    Record();
    
    Record(Timestamp *tBegin, Timestamp *tEnd, int object, int value);

    // Destructor
    ~Record();

    Timestamp * getBegin();
    Timestamp * getEnd();
    int getObject();
    int getValue();
    Record * getNextRecord();
    
    void setBegin(Timestamp *t);
    void setEnd(Timestamp *t);
    void setNextRecord(Record *r);
private:
    Timestamp *_begin;
    Timestamp *_end;
    int _object;
    int _value;
    Record *_nextRecord;
};

#endif
