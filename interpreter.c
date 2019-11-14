#include "includes.h"

int main(void)
{
	int i = 0;
	strcpy(state, "00");
	strcpy(statePrevious, state);
	maximize();
	hideCursor();
	
	for (i = 0; i <= 15; i++)
	{
		registersNone[i] = -1;
		registersNonePrevious[i] = -1;
		toChange[i] = 1;
	}
	
	//call to core.h function
	readInput();
	moveTo(max(40, 23 + stackPointer) + 3, 0);
}
