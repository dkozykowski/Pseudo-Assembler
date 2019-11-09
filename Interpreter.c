#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "GUI.h"
#include "core.h"
#define showContent 1 //if equal to 1 - whole tables' content is leftSideed; if 0 - just indicator to its' first element
#define firstAddress 100


struct singleCommand input[300];
struct variable memory[300];
int inputSize, memoryStack[100000], stackPointer, varNumber, registers[20], registersPrevious[20], registersNone[20], registersNonePrevious[20], line, toChange[30], memoryStackPrevious[100000], varNumberPrevious[1000];
char state[10], statePrevious[10];



int main(void)
{
	strcpy(state, "00");
	strcpy(statePrevious, state);
	maximize();
	hideCursor();
	for (int i = 0; i <= 15; i++) registersNone[i] = -1, registersNonePrevious[i] =- 1, toChange[i] = 1;
	readInput(&inputSize, &varNumber, &input[0], &registers[0], &registersPrevious[0], &registersNone[0], &registersNonePrevious[0], &toChange[0], &state[0], &statePrevious[0], &memory[0], &memoryStack[0], &memoryStackPrevious[0], &varNumberPrevious[0], &stackPointer);
	int k = -1;
	leftSide(&k, &inputSize, &varNumber, &input[0], &registers[0], &registersPrevious[0], &registersNone[0], &registersNonePrevious[0], &toChange[0], &state[0], &statePrevious[0], &memory[0], &memoryStack[0], &memoryStackPrevious[0], &varNumberPrevious[0]);
	lineByLine(&inputSize, &varNumber, &input[0], &registers[0], &registersPrevious[0], &registersNone[0], &registersNonePrevious[0], &toChange[0], &state[0], &statePrevious[0], &memory[0], &memoryStack[0], &memoryStackPrevious[0], &varNumberPrevious[0], &stackPointer);
	moveTo(max(40, 23 + stackPointer) + 3, 0);
}
