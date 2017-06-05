#ifndef HECKADBMS_OPERATION_H
#define HECKADBMS_OPERATION_H

#include <vector>
#include <random>

using namespace std;

class Operation 
{
public:
	/// Enumeration defining OpMode states
	enum Mode {
		READ,
		WRITE
	};

	/// Default Constructor
	Operation();

	/// Constructor taking a mode and key
    Operation(Mode mode, int key);

	/// Constructor taking a mode, key, and value
	Operation(Mode mode, int key, int value);

	/// Get the mode of the operation
	Mode getMode();

	/// Get entry key of the data object
	int getKey();

	/// Get the value being written if the Mode is WRITE
	int getValue();

	/// Set the mode of the operation
	void setMode(Mode mode);

	/// Set the entry key of the data object
	void setKey(int key);

	/// Set the value being written if the Mode is WRITE
	void setValue(int value);

private:
	Mode _mode;
	int _key;
	int _value;
};

#endif