#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include "memsim.h"
#include "instructions.h"
#include "pagetable.h"

#define MAX_GETLINE_CHARS 128

int InputStrToInt(char* inStr, int* outInt);

bool Input_NextInstruction(char* line);

int Input_GetLine(char** line);

void InputDispatchCommand(int pid, char* instruction_type, int virtual_address, int value);

int InputParseAndValidateLine(char* line, int* pidOut, char** instructionTypeOut, int* VAOut, int* valOut);