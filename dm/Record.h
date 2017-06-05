#ifndef HECKADBMS_RECORD_H
#define HECKADBMS_RECORD_H

#include "Timestamp.h"

class Record
{
public:
    /// Default constructor
    Record();

    /// Constructor that takes values for the entry key, object key, and object value as parameters
    Record(int entryKey, int objectKey, int objectValue);

    /// Constructor that takes timestamp pointers and values for the entry key, object key, and object value as parameters
    Record(Timestamp *tBegin, Timestamp *tEnd, int entryKey, int objectKey, int objectValue);

    /// Get the key of the data entry
    int getEntryKey();

    /// Get whether this Record is the latest (most valid) version of the data object
    bool getIsLatest();

    /// Get the key of the data object
    int getObjectKey();

    /// Get the value of the data object
    int getObjectValue();

    /// Get the beginning timestamp
    Timestamp *getBegin();

    /// Get the ending timestamp
    Timestamp *getEnd();

    /// Get the pointer to the next record (if there is one)
    Record* getNextRecord();

    /// Set the key of the data entry
    void setEntryKey(int entryKey);

    /// Set whether this Record is the latest (most valid) version of the data object
    void setIsLatest(bool isLatest);

    /// Set the key of the data object
    void setObjectKey(int objectKey);

    /// Set the value of the data object
    void setObjectValue(int objectValue);

    /// Set the beginning timestamp
    void setBegin(Timestamp *begin);

    /// Set the ending timestamp
    void setEnd(Timestamp *end);

    /// Set the pointer to the next record (if there is one)
    void setNextRecord(Record *nextRecord);

    Record *getMostRecentRecord(int key);

private:
    int _entryKey;
    bool _isLatest;
    int _objectKey;
    int _objectValue;
    Timestamp *_begin;
    Timestamp *_end;
    Record *_nextRecord;
};

#endif
