FILE *commands;

struct singleCommand input[300];
struct variable memory[300];

int inputSize, stackPointer, varNumber;
int memoryStack[100000], memoryStackPrevious[100000];
int registers[20], registersPrevious[20];
int registersNone[20], registersNonePrevious[20];
int toChange[30];
int varNumberPrevious[1000];

char state[10], statePrevious[10];


