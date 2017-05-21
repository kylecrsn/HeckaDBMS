#ifndef HECKADBMS_RECORD_H
#define HECKADBMS_RECORD_H

#include "Timestamp.h"

class Record
{
public:
    // Default constructor
    Record();

    // Destructor
    ~Record();

private:
    Timestamp _begin;
    Timestamp _end;
    int _object;
    int _value;
    Record *_nextRecord;
};

#endif
