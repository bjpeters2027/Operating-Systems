// Starting code version 1.0

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "input.h"
#include "memsim.h"
#include "instructions.h"
#include "pagetable.h"


/* Private Internals */

int InputStrToInt(char* inStr, int* outInt) {
	int bytesCast;
	bytesCast = sscanf(inStr, "%d", outInt);
	if (bytesCast != 1) {
		// printf("Incorrectly formatted instruction.\n" \
		// 		  "Correct format is: process_id,instruction_type,virtual_address,value\n");
		*outInt = -1;
	}
	return true;
}

void InputDispatchCommand(int pid, char* instruction_type, int virtual_address, int value) {
	// dispatch to the appropriate instruction handler
	if (strcmp(instruction_type, "map") == 0) {
		Instruction_Map(pid, virtual_address, value);
	} else if (strcmp(instruction_type, "store") == 0) {
		Instruction_Store(pid, virtual_address, value);
	} else if (strcmp(instruction_type, "load") == 0) {
		if(value != -1){
			printf("Incorrectly formatted instruction.\nValue should be NA for the load instruction.\n");
		}
		else{
			Instruction_Load(pid, virtual_address);
		}
	} else{
		printf("Invalid Instruction type. Valid instructions are map, store, and load.\n");
	}
}

int InputParseAndValidateLine(char* line, int* pidOut, char** instructionTypeOut, int* VAOut, int* valOut) {
	//if there is input, process it
	char* pid_string;
	char* virtual_address_string;
	char* value_string;
	
	//split instruction into its 4 parts (still strings)
	pid_string = strtok(line, ",");
	*instructionTypeOut = strtok(NULL, ",");
	virtual_address_string = strtok(NULL, ",");
	value_string = strtok(NULL, "\n");

	//convert string containing pid to an int
	if (!InputStrToInt(pid_string, pidOut)) {
		return false;
	} else { // validate integer value of pid
		if (*pidOut < 0 || *pidOut > NUM_PROCESSES-1) {
			printf("Invalid Process Id.  Process Id must be in range 0-3.\n");
			return false;
		}
	}

	//convert string containing virtual address to an int
	if (!InputStrToInt(virtual_address_string, VAOut)) {
		return false;
	} else {
		if (*VAOut < 0 || *VAOut > VIRTUAL_SIZE-1) { // validate integer value of virtual address
			printf("Invalid Virtual Address.  Virtual Address must be in range 0-63.\n");
			return false;
		}
	}

	//convert string containing value to an int, check for NA in case of load instruction
	//further value validation is done by the instruction implementations
	if(strcmp(value_string,"NA") == 0){
		*valOut = -1;
	} else {
		if (!InputStrToInt(value_string, valOut)) {
			return false;
		}
	}
}


/*
 * Public Interface: 
 */

bool Input_NextInstruction(char* line) {
	// integer values of the instruction
	int pid;
	char* instruction_type;
	int virtual_address;
	int value;

	// load validated values into the instruction variables, or return and try again
	if (!InputParseAndValidateLine(line, &pid, &instruction_type, &virtual_address, &value)) {
		return false;
	} 

	// dispatch the instruction to the appropriate handler
	InputDispatchCommand(pid, instruction_type, virtual_address, value);
	return true; // successful instruction execution
}