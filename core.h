#include <windows.h>
#include <stdio.h>
#define shift 30
#define showContent 1 //if equal to 1 - whole tables' content is leftSideed; if 0 - just indicator to its' first element
#define firstAddress 100
FILE *commands;

//updates color of old variables from red to white
void update(int k, int *registers, int *registersPrevious, int *registersNone, int *registersNonePrevious, int *toChange, char state[10], int *varNumber, int *varNumberPrevious, int *memoryStack, int *memoryStackPrevious, struct variable *memory, char statePrevious[10])
{
	int line = -1;
	for (int i = 0; i <= 15; i++)
	{
		if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i])
		{
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 1;
		}
		else if (toChange[i])
		{
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 0;
		}
	}
	strcpy(statePrevious, state);
	for (int i = 0; i < *varNumber; i++)
	{
		line ++;
		if (i == k) {}
		else if (varNumberPrevious[i] == 1)
		{ 
			varNumberPrevious[i] = -1;
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)	memoryStackPrevious[memory[i].firstIndex] = -1;
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
					if (memoryStackPrevious[o] == 1) memoryStackPrevious[o] = -1;
			}
			else if (memoryStackPrevious[memory[i].firstIndex] == 1) memoryStackPrevious[memory[i].firstIndex] = -1;
		}
		else if (varNumberPrevious[i] == -1)
		{ 
			varNumberPrevious[i] = 0;
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)	memoryStackPrevious[memory[i].firstIndex] = 0;
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
					if (memoryStackPrevious[o] == 1) memoryStackPrevious[o] = 0;
			}
			else if (memoryStackPrevious[memory[i].firstIndex] == 1)	memoryStackPrevious[memory[i].firstIndex] = 0;
		}
	}
}

//checks if label is a function type
int check(int k, struct singleCommand *input)
{
	if (strcmp(input[k].label, "A") == 0 || \
	strcmp(input[k].label, "AR")    == 0 || \
	strcmp(input[k].label, "S")     == 0 || \
	strcmp(input[k].label, "SR")    == 0 || \
	strcmp(input[k].label, "M")     == 0 || \
	strcmp(input[k].label, "MR")    == 0 || \
	strcmp(input[k].label, "D")     == 0 || \
	strcmp(input[k].label, "DR")    == 0 || \
	strcmp(input[k].label, "J")     == 0 || \
	strcmp(input[k].label, "JZ")    == 0 || \
	strcmp(input[k].label, "JP")    == 0 || \
	strcmp(input[k].label, "JN")    == 0 || \
	strcmp(input[k].label, "L")     == 0 || \
	strcmp(input[k].label, "LA")    == 0 || \
	strcmp(input[k].label, "LR")    == 0 || \
	strcmp(input[k].label, "C")     == 0 || \
	strcmp(input[k].label, "CR")    == 0 || \
	strcmp(input[k].label, "ST")    == 0) return 1;
	return 0;
}

//reads given file line by line and creates an array of struct commands
void readInput(int *inputSize, int *varNumber, struct singleCommand *input, int *registers, int *registersPrevious, int *registersNone, int *registersNonePrevious, int *toChange, char state[10], char statePrevious[10], struct variable *memory, int *memoryStack, int *memoryStackPrevious, int *varNumberPrevious, int *stackPointer)
{
	commands = fopen("commands.txt", "r");
	char inputLine[1000];
	int argIndex = 0; 
	int charIndex = 0; 
	
	while(fgets(inputLine, 1000, commands) != NULL)
	{
		if (inputLine[0] == '\n') continue;
		argIndex = charIndex = 0;
		for (int i = 0; (i < (int)strlen(inputLine) && argIndex < 4); i++)
		{
			if (('A' <= inputLine[i] && inputLine[i] <= 'Z') || ('0' <= inputLine[i] && inputLine[i] <= '9') || inputLine[i] == '-' || inputLine[i] == '*' || inputLine[i] == '(' || inputLine[i] == ')' || inputLine[i] == '_')
			{
				if (argIndex == 0)
					input[*inputSize].label[charIndex] = inputLine[i];
					
				if (argIndex == 1)
					input[*inputSize].type[charIndex] = inputLine[i];
					
				if (argIndex == 2)
					input[*inputSize].argument1[charIndex] = inputLine[i];
					
				if (argIndex == 3)
					input[*inputSize].argument2[charIndex] = inputLine[i];
				charIndex++;
			}	
			else if (charIndex > 0)
			{
				argIndex++;
				charIndex = 0;
			}
			if (argIndex == 1 && check(*inputSize, input))
			{
				strcpy(input[*inputSize].type, input[*inputSize].label);
				strcpy(input[*inputSize].label, "");
				argIndex++;
			}
		}
		(*inputSize)++;
	}
	fclose(commands);
}


//<label> DC <arg1> <arg2>
//allocates memory with value
void DC(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{	
	strcpy((*(memory + *varNumber)).label, input[i].label);
	(*(memory + *varNumber)).firstIndex = *stackPointer;
	int negative = 1;
	int howMuch1 = 0;
	if ((*(input + i)).argument1[(int)strlen((*(input + i)).argument1) - 1] == ')')
	{
		
		int power10 = 1;
		for (int o = (int)strlen((*(input + i)).argument1) - 2; (*(input + i)).argument1[o] != '('; o--)
		{
			if ((*(input + i)).argument1[o] == '-')
			{
				negative = -1;
				break;
			}
			howMuch1 += power10 * ((*(input + i)).argument1[o] - '0');
			power10 *= 10;
		}
	}
	int howMuch = 0;
	moveTo(1, 10);
	for (int o = 0; (*(input + i)).argument1[o] != '*'; o++)
	{
		if (!('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')) break;
		howMuch *= 10;
		howMuch += (*(input + i)).argument1[o] - '0';
	}
	howMuch = max(1, howMuch);
	howMuch += *stackPointer;
	for (;*stackPointer < howMuch; (*stackPointer)++) memoryStack[*stackPointer] = negative * howMuch1;
	(*(memory + *varNumber)).lastIndex = *stackPointer;
	(*varNumber)++;
}

//<label> DS <arg1>
//allocates memory without value
void DS(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	strcpy((*(memory + *varNumber)).label, (*(input + i)).label);
	(*(memory + *varNumber)).firstIndex = *stackPointer;
	if ((*(input + i)).argument1[0] != 'I')
	{
		int howMuch = 0;
		for (int o = 0; (*(input + i)).argument1[o] != '*'; o++)
		{
			howMuch *= 10;
			howMuch += (*(input + i)).argument1[o] - '0';
		}
		stackPointer += howMuch;
	}
	else (*stackPointer)++;
	(*(memory + *varNumber)).lastIndex = *stackPointer;
	(*varNumber)++;
}

//<label> A <arg1> <arg2>
//adds memory with index arg2 to register with index arg2
void A(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] += index2;
	registersNone[index] = 0;
}

//<label> AR <arg1> <arg2>
//adds register with index arg2 to register with index arg2
void AR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	registers[index] += registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> S <arg1> <arg2>
//subtracts register with index arg2 by memory with index arg2
void S(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] -= index2;
	registersNone[index] = 0;
}

//<label> SR <arg1> <arg2>
//subtracts register with index arg2 by register with index arg2
void SR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	registers[index] -= registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> M <arg1> <arg2>
//multiplies register with index arg2 by memory with index arg2
void M(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] *= index2;
	registersNone[index] = 0;
}

//<label> MR <arg1> <arg2>
//multiplies register with index arg2 by register with index arg2
void MR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	registers[index] *= registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> D <arg1> <arg2>
//devides register with index arg2 by memory with index arg2
void D(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] /= index2;
	registersNone[index] = 0;
}

//<label> DR <arg1> <arg2>
//devides register with index arg2 by register with index arg2
void DR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	registers[index] /= registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> C <arg1> <arg2>
//compares register with index arg1 with memory with index with arg2
//01 if arg1 > arg2
//00 if arg1 == arg2
//10 if arg1 < arg2
void C(int i, struct singleCommand *input, int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, char state[10], int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	
	moveTo(0, 0);
	clear();
	if (registers[index] - index2 > 0) strcpy(state, "01");
	else if (registers[index] - index2 < 0) strcpy(state, "10");
	else if (registers[index] - index2 == 0) strcpy(state, "00");
}

//<label> CR <arg1> <arg2>
//compares register with index arg1 with register with index with arg2
//01 if arg1 > arg2
//00 if arg1 == arg2
//10 if arg1 < arg2
void CR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, char state[10], int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	
	if (registers[index] - registers[index2] > 0) strcpy(state, "01");
	else if (registers[index] - registers[index2] < 0) strcpy(state, "10");
	else if (registers[index] - registers[index2] == 0) strcpy(state, "00");
}

//<label> J arg1
//jumps to line with label == arg1 
int J(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *inputSize, char state[10])
{
	for (int k = 0; k < *inputSize; k++)
		if (strcmp(input[k].label, (*(input + i)).argument1) == 0) return k;
	return i + 1;
}

//<label> JZ arg1
//if program state == 00: jumps to line with label == arg1
int JZ(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *inputSize, char state[10])
{
	if (strcmp(state, "00") == 0)
		for (int k = 0; k < *inputSize; k++)
			if (strcmp(input[k].label, (*(input + i)).argument1) == 0) return k;
	return i + 1;
}

//<label> JP arg1
//if program state == 01: jumps to line with label == arg1
int JP(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *inputSize, char state[10])
{
	if (strcmp(state, "01") == 0)
		for (int k = 0; k < *inputSize; k++)
			if (strcmp(input[k].label, (*(input + i)).argument1) == 0) return k;
	return i + 1;
}

//<label> JN arg1
//if program state == 10: jumps to line with label == arg1
int JN(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *inputSize, char state[10])
{
	if (strcmp(state, "10") == 0)
		for (int k = 0; k < *inputSize; k++)
			if (strcmp(input[k].label, (*(input + i)).argument1) == 0) return k;
	return i + 1;
}

//<label> L <arg1> <arg2>
//loads memory with index arg2 value to register with index arg1
void L(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] = index2;
	registersNone[index] = 0;
}

//<label> LA <arg1> <arg2>
//loads memory with index arg2 address to register with index arg1
void LA(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
			if (strcmp((*(memory + o)).label, temporaryLabel) == 0) 
				index2 = (*(memory + o)).firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen((*(input + i)).argument2) - 1);
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if((*(input + i)).argument2[(int)strlen((*(input + i)).argument2) - 1] == ')') index2 = registers[howMuch] / 4 + index2;
	else index2 = index2;
	registers[index] = index2 * 4 + firstAddress;
	registersNone[index] = 0;
}

//<label> LR <arg1> <arg2>
//loads register with index arg2 value to register with index arg1
void LR(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument2[o] && (*(input + i)).argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[o] - '0';
		}
		else break;
	}
	registers[index] = registers[index2];
	registersNone[index] = 0;
}

//<label> ST <arg1> <arg2>
//loads register with index arg1 value to memory with index arg2
void ST(int i, struct singleCommand *input,  int *varNumberPrevious, int *registers, int *memoryStack, int *memoryStackPrevious, int *registersNone, int *stackPointer, struct variable *memory, int *varNumber)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= (*(input + i)).argument1[o] && (*(input + i)).argument1[o] <= '9')
		{
			index *= 10;
			index += (*(input + i)).argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= (*(input + i)).argument2[0] && (*(input + i)).argument2[0] <= '9')
	{
		for (; (z < (int)strlen((*(input + i)).argument2) && (*(input + i)).argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += (*(input + i)).argument2[z] - '0';
		}
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen((*(input + i)).argument2); z++)
		{
			if ((*(input + i)).argument2[z] == '(') break;
			temporaryLabel[z] = (*(input + i)).argument2[z];
		}
		for (int o = 0; o < *varNumber; o++)
		if (strcmp((*(memory + o)).label, temporaryLabel) == 0)
		{ 
			index2 = (*(memory + o)).firstIndex;
			varNumberPrevious[o] = 1;
		}
		index2 = index2 * 4 + 100;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen((*(input + i)).argument2) - 1; z++)
	{
		howMuch *= 10;
		howMuch += (*(input + i)).argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = (registers[howMuch] + index2 - 100) /4;
	else index2 = (index2 - 100) / 4;
	*(memoryStackPrevious + index2) = 1;
	*(memoryStack + index2) = *(registers + index);
}

//calls function to operate every single line of input in terminal one by one
void lineByLine(int *inputSize, int *varNumber, struct singleCommand *input, int *registers, int *registersPrevious, int *registersNone, int *registersNonePrevious, int *toChange, char state[10], char statePrevious[10], struct variable *memory, int *memoryStack, int *memoryStackPrevious, int *varNumberPrevious, int *stackPointer)
{
	for (int i = 0; i < *inputSize; i++)
	{
		int z = i + 1;
		if (strcmp((*(input + i)).type,      "DC") == 0)     DC(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "DS") == 0)     DS(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "A")  == 0)      A(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "AR") == 0)     AR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "S")  == 0)      S(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "SR") == 0)     SR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "M")  == 0)      M(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "MR") == 0)     MR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "D")  == 0)      D(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "DR") == 0)     DR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "C")  == 0)      C(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, state, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "CR") == 0)     CR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, state, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "J")  == 0)  z = J(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, inputSize, state);
		else if (strcmp((*(input + i)).type, "JZ") == 0) z = JZ(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, inputSize, state);
		else if (strcmp((*(input + i)).type, "JP") == 0) z = JP(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, inputSize, state);
		else if (strcmp((*(input + i)).type, "JN") == 0) z = JN(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, inputSize, state);
		else if (strcmp((*(input + i)).type, "L")  == 0)      L(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "LA") == 0)     LA(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "LR") == 0)     LR(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		else if (strcmp((*(input + i)).type, "ST") == 0)     ST(i, input, varNumberPrevious, registers, memoryStack, memoryStackPrevious, registersNone, stackPointer, memory, varNumber);
		display(i, inputSize, varNumber, input, registers, registersPrevious, registersNone, registersNonePrevious, toChange, state, statePrevious, memory, memoryStack, memoryStackPrevious, varNumberPrevious);
		update(i, registers, registersPrevious, registersNone, registersNonePrevious, toChange, state, varNumber, varNumberPrevious, memoryStack, memoryStackPrevious, memory, statePrevious);
		i = z - 1;
		char x[1000];
		gets(x);
	}
}

