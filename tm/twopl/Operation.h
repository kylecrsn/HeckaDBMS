#ifndef HECKADBMS_OPERATION_H
#define HECKADBMS_OPERATION_H



class Operation 
{

public:
	Operation();
	
	enum OpMode {
    	NONE,
        READ,
        WRITE
    };
    
    Operation(OpMode mode, int key);
	
	Operation(OpMode mode, int key, int value);

private:
	OpMode mode;
	int objectKey;
	int objectValue;

};

#endif