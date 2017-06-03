#include "Operation.h"

Operation::Operation() {
	OpMode mode = NONE;
	int objectKey = 0;
	int objectValue = 0;
}

Operation::Operation(OpMode mode, int key) {
	OpMode mode = READ
	int objectKey = key;
	int objectValue = 0;
}

Operation::Operation(OpMode mode, int key, int value) {
	OpMode mode = WRITE;
	int objectKey = key;
	int objectValue = value;
}