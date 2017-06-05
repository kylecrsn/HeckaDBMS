#include "Operation.h"

Operation::Operation() {
	_mode = READ;
    _key = -1;
    _value = -1;
}

Operation::Operation(Mode mode, int key) {
	_mode = mode;
	_key = key;
	_value = -1;
}

Operation::Operation(Mode mode, int key, int value) {
    _mode = mode;
    _key = key;
    _value = value;
}

Operation::Mode Operation::getMode() {
    return _mode;
}

int Operation::getKey() {
    return _key;
}

int Operation::getValue() {
    return _value;
}

void Operation::setMode(Operation::Mode mode) {
    _mode = mode;
}

void Operation::setKey(int key) {
    _key = key;
}

void Operation::setValue(int value) {
    _value = value;
}