#include <stdio.h>
#include <string.h>
#include <windows.h>
#define shift 30
#define showContent 1 //if equal to 1 - whole tables' content is leftSideed; if 0 - just indicator to its' first element
#define firstAddress 100

struct variable
{
	char label[30];
	int firstIndex;
	int lastIndex;
};

struct singleCommand
{
	char label[30]; 
	char type[5]; 
	char argument1[30];
	char argument2[30]; 
};

FILE *commands;
struct singleCommand input[300];
struct variable memory[300];
int inputSize, memoryStack[100000], stackPointer, varNumber, registers[20], registersPrevious[20], registersNone[20], registersNonePrevious[20], line, toChange[30], memoryStackPrevious[100000], varNumberPrevious[1000];
char state[10], statePrevious[10];

//hides blinking cursor
void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

//clears some space in a row from trash
void clear(void)
{
	for (int z = 0; z < 70; z++) printf(" ");
	printf("\r");
}

//maximizes terminal window
void maximize(void)
{
    HWND consoleWindow = GetConsoleWindow(); 
    ShowWindow(consoleWindow, SW_MAXIMIZE); 
}

//moves cursor to position (x, y)
void moveTo(int y, int x)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsole, pos);
}

//changes color of characters in terminal
void color(int k)
{
	WORD kolor;
	if (k == 0)	kolor = 0x000f; //white
	if (k == 1) kolor = 0x000a; //green
	if (k == 2) kolor = 0x000e; //yellow
	if (k == 3) kolor = 0x000c; //red
	HANDLE manual;
	manual = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(manual,kolor);
}

//leftSides right side of terminal
void rightSide(int k)
{
	color(2);
	moveTo(2, 80 + shift);
	printf("Commands: \n\n");
	moveTo(4, 80 + shift);
	printf("Line nr:   Label:       Type:    Argument1:             Argument2:\n");
	color(0);
	int start, stop;
	int lineNumber = 0;
	if (inputSize <= 40)
	{
		start = 0;
		stop = inputSize;
	}
	else if (k <= 20)
	{
		start = 0;
		stop = 40;
	}
	else if (20 < k && k + 20 <= inputSize)
	{
		start = k - 20;
		stop = k + 20;
	}
	else if (k + 20 > inputSize)
	{
		start = inputSize - 40;
		stop = inputSize;
	}
	for (int i = start; i < stop; i++)
	{
		lineNumber ++;
		if (i == k)
		{
			moveTo(5 + lineNumber - 1, 60 + shift);
			clear();
			color(1);
			moveTo(5 + lineNumber - 1, 65 + shift);
			printf("current line ->");
			printf("%d", i + 1);
			if (strcmp(input[i].label, "")) 
			{
				moveTo(5 + lineNumber - 1, 91 + shift);
				printf("%s", input[i].label);
			}
			else
			{
				moveTo(5 + lineNumber - 1, 91 + shift);
				printf("<NONE>");
			}
			
			moveTo(5 + lineNumber - 1, 104 + shift);
			printf("%s", input[i].type);
			
			moveTo(5 + lineNumber - 1, 113 + shift);
			printf("%s", input[i].argument1);
			
			moveTo(5 + lineNumber - 1, 136 + shift);
			if (strcmp(input[i].argument2, "")) printf("%s\n", input[i].argument2);
			else printf("<NONE>\n");
			color(0);
		}
		else
		{
			moveTo(5 + lineNumber - 1, 60 + shift);
			clear();
			moveTo(5 + lineNumber - 1, 80 + shift);
			printf("%d", i + 1);
			
			if (strcmp(input[i].label, "")) 
			{
				
				moveTo(5 + lineNumber - 1, 91 + shift);
				printf("%s", input[i].label);			
			}
			else
			{
				moveTo(5 + lineNumber - 1, 91 + shift);
				printf("<NONE>");
			}

			moveTo(5 + lineNumber - 1, 104 + shift);
			printf("%s", input[i].type);
			
			moveTo(5 + lineNumber - 1, 113 + shift);
			printf("%s", input[i].argument1);
			
			moveTo(5 + lineNumber - 1, 136 + shift);
			if (strcmp(input[i].argument2, "")) printf("%s\n", input[i].argument2);
			else printf("<NONE>\n");
		}
	}
}

//displays left side of terminal
void leftSide(int k) 
{
	color(2);
	moveTo(2, 0);
	puts("Registers:\n");
	printf("Index:       Value:\n");
	color(0);
	for (int i = 0; i <= 15; i++)
	{
		if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i])
		{
			color(3);
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i] == -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 1;
			color(0);
		}
		else if (toChange[i])
		{
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i] == -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 0;
		}
		else printf("\n");
	}
	color(2);
	printf("\nProgram state: ");
	if (strcmp(state, statePrevious) == 0) color(0);
	else color(3);
	printf("%s", state);
	strcpy(statePrevious, state);
	
	color(2);
	puts("\n\nMemory:\n");
	puts("Address:     Variable:    Label:");
	color(0);
	line = -1;
	for (int i = 0; i < varNumber; i++)
	{
		line ++;
		if (i == k)
		{ 
			color(3);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);

				} 
			}
			
			else //if veriable is a single int
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
			color(0);
		}
		else if (varNumberPrevious[i] == 1)
		{ 
			varNumberPrevious[i] = -1;
			color(0);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)
				{
					color(3);
					memoryStackPrevious[memory[i].firstIndex] = -1;
				}
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				color(0);
				
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					
					if (memoryStackPrevious[o] == 1)
					{
						color(3);
						memoryStackPrevious[o] = -1;
					}
					
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);
					color(0);
				} 
			}
			
			else //if veriable is a single int
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)
				{
					color(3);
					memoryStackPrevious[memory[i].firstIndex] = -1;
				}
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
				color(0);
			}
			color(0);
		}
		else if (varNumberPrevious[i] == -1)
		{ 
			varNumberPrevious[i] = 0;
			color(0);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)	memoryStackPrevious[memory[i].firstIndex] = 0;
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					
					if (memoryStackPrevious[o] == 1) memoryStackPrevious[o] = 0;
					
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);
				} 
			}
			
			else //if veriable is a single int
			{
				if (memoryStackPrevious[memory[i].firstIndex] == 1)	memoryStackPrevious[memory[i].firstIndex] = 0;
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
		}
		else if (i == k - 1)
		{
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);

				} 
			}
			
			else //if veriable is a single int
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
		}
		else
		{
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				printf("\n");
				for (int o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("\n");
				} 
			}
			else //if veriable is a single int
			{
				printf("\n");
			}
		}
	}
	color(2);
	rightSide(k);
	color(2);
	if (k + 1 == inputSize)
	{
		moveTo(0, 50);
		clear();
		moveTo(0, 70);
		printf("<press enter to close>");
	} 
	else
	{
		moveTo(0, 60);
		printf("<press enter to proceed to next line>");
	} 
	color(0);
	moveTo(0, 0);
	char x[1000];
	gets(x);
}

//checks if label is a function type
int check(int k)
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
void readInput(void)
{
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
					input[inputSize].label[charIndex] = inputLine[i];
					
				if (argIndex == 1)
					input[inputSize].type[charIndex] = inputLine[i];
					
				if (argIndex == 2)
					input[inputSize].argument1[charIndex] = inputLine[i];
					
				if (argIndex == 3)
					input[inputSize].argument2[charIndex] = inputLine[i];
				charIndex++;
			}	
			else if (charIndex > 0)
			{
				argIndex++;
				charIndex = 0;
			}
			if (argIndex == 1 && check(inputSize))
			{
				strcpy(input[inputSize].type, input[inputSize].label);
				strcpy(input[inputSize].label, "");
				argIndex++;
			}
		}
		inputSize++;
	}
}

//<label> DC <arg1> <arg2>
//allocates memory with value
void DC(int i)
{
	strcpy(memory[varNumber].label, input[i].label);
	memory[varNumber].firstIndex = stackPointer;
	int negative = 1;
	int howMuch1 = 0;
	if (input[i].argument1[(int)strlen(input[i].argument1) - 1] == ')')
	{
		
		int power10 = 1;
		for (int o = (int)strlen(input[i].argument1) - 2; input[i].argument1[o] != '('; o--)
		{
			if (input[i].argument1[o] == '-')
			{
				negative = -1;
				break;
			}
			howMuch1 += power10 * (input[i].argument1[o] - '0');
			power10 *= 10;
		}
	}
	int howMuch = 0;
	for (int o = 0; input[i].argument1[o] != '*'; o++)
	{
		howMuch *= 10;
		howMuch += input[i].argument1[o] - '0';
	}
	howMuch = max(1, howMuch);
	howMuch += stackPointer;
	for (;stackPointer < howMuch; stackPointer++) memoryStack[stackPointer] = negative * howMuch1;
	memory[varNumber].lastIndex = stackPointer;
	varNumber++;
}

//<label> DS <arg1>
//allocates memory without value
void DS(int i)
{
	strcpy(memory[varNumber].label, input[i].label);
	memory[varNumber].firstIndex = stackPointer;
	if (input[i].argument1[0] != 'I')
	{
		int howMuch = 0;
		for (int o = 0; input[i].argument1[o] != '*'; o++)
		{
			howMuch *= 10;
			howMuch += input[i].argument1[o] - '0';
		}
		stackPointer += howMuch;
	}
	else stackPointer++;
	memory[varNumber].lastIndex = stackPointer;
	varNumber++;
}

//<label> A <arg1> <arg2>
//adds memory with index arg2 to register with index arg2
void A(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] += index2;
	registersNone[index] = 0;
}

//<label> AR <arg1> <arg2>
//adds register with index arg2 to register with index arg2
void AR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	registers[index] += registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> S <arg1> <arg2>
//subtracts register with index arg2 by memory with index arg2
void S(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] -= index2;
	registersNone[index] = 0;
}

//<label> SR <arg1> <arg2>
//subtracts register with index arg2 by register with index arg2
void SR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	registers[index] -= registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> M <arg1> <arg2>
//multiplies register with index arg2 by memory with index arg2
void M(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] *= index2;
	registersNone[index] = 0;
}

//<label> MR <arg1> <arg2>
//multiplies register with index arg2 by register with index arg2
void MR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	registers[index] *= registers[index2];
	registersNone[index] = registersNone[index2] = 0;
}

//<label> D <arg1> <arg2>
//devides register with index arg2 by memory with index arg2
void D(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] /= index2;
	registersNone[index] = 0;
}

//<label> DR <arg1> <arg2>
//devides register with index arg2 by register with index arg2
void DR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
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
void C(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	
	if (registers[index] - index2 > 0) strcpy(state, "01");
	else if (registers[index] - index2 < 0) strcpy(state, "10");
	else if (registers[index] - index2 == 0) strcpy(state, "00");
}

//<label> CR <arg1> <arg2>
//compares register with index arg1 with register with index with arg2
//01 if arg1 > arg2
//00 if arg1 == arg2
//10 if arg1 < arg2
void CR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	
	if (registers[index] - registers[index2] > 0) strcpy(state, "01");
	else if (registers[index] - registers[index2] < 0) strcpy(state, "10");
	else if (registers[index] - registers[index2] == 0) strcpy(state, "00");
}

//<label> J arg1
//jumps to line with label == arg1 
int J(int i)
{
	for (int k = 0; k < inputSize; k++)
		if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i;
}

//<label> JZ arg1
//if program state == 00: jumps to line with label == arg1
int JZ(int i)
{
	if (strcmp(state, "00") == 0)
		for (int k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i;
}

//<label> JP arg1
//if program state == 01: jumps to line with label == arg1
int JP(int i)
{
	if (strcmp(state, "01") == 0)
		for (int k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i;
}

//<label> JN arg1
//if program state == 10: jumps to line with label == arg1
int JN(int i)
{
	if (strcmp(state, "10") == 0)
		for (int k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i;
}

//<label> L <arg1> <arg2>
//loads memory with index arg2 value to register with index arg1
void L(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
	else index2 = memoryStack[index2];
	registers[index] = index2;
	registersNone[index] = 0;
}

//<label> LA <arg1> <arg2>
//loads memory with index arg2 address to register with index arg1
void LA(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		//printf("%d %d\n", z, (int)strlen(input[i].argument2) - 1);
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = registers[howMuch] / 4 + index2;
	else index2 = index2;
	registers[index] = index2 * 4 + firstAddress;
	registersNone[index] = 0;
}

//<label> LR <arg1> <arg2>
//loads register with index arg2 value to register with index arg1
void LR(int i)
{
	int index = 0;
	int index2 = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	
	registers[index] = registers[index2];
}

//<label> ST <arg1> <arg2>
//loads register with index arg1 value to memory with index arg2
void ST(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	for (int o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < (int)strlen(input[i].argument2) && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	else
	{
		char x = 0;	
		for (int z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (; z < (int)strlen(input[i].argument2); z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (int o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0)
			{ 
				index2 = memory[o].firstIndex;
				varNumberPrevious[o] = 1;
			}										
	}
	z ++;
	int howMuch = 0;
	for (; z < (int)strlen(input[i].argument2) - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	if(input[i].argument2[(int)strlen(input[i].argument2) - 1] == ')') index2 = registers[howMuch] / 4 + index2;
	else index2 = index2;
	
	memoryStackPrevious[index2] = 1;
	memoryStack[index2] = registers[index];
}

//shows differences made by single line of input in terminal
void lineByLine(void)
{
	for (int i = 0; i < inputSize; i++)
	{
		strcpy(state, "00");
		if (strcmp(input[i].type,      "DC") == 0)     DC(i);
		else if (strcmp(input[i].type, "DS") == 0)     DS(i);
		else if (strcmp(input[i].type, "A")  == 0)      A(i);
		else if (strcmp(input[i].type, "AR") == 0)     AR(i);
		else if (strcmp(input[i].type, "S")  == 0)      S(i);
		else if (strcmp(input[i].type, "SR") == 0)     SR(i);
		else if (strcmp(input[i].type, "M")  == 0)      M(i);
		else if (strcmp(input[i].type, "MR") == 0)     MR(i);
		else if (strcmp(input[i].type, "D")  == 0)      D(i);
		else if (strcmp(input[i].type, "DR") == 0)     DR(i);
		else if (strcmp(input[i].type, "C")  == 0)      C(i);
		else if (strcmp(input[i].type, "CR") == 0)     CR(i);
		else if (strcmp(input[i].type, "J")  == 0)  i = J(i);
		else if (strcmp(input[i].type, "JZ") == 0) i = JZ(i);
		else if (strcmp(input[i].type, "JP") == 0) i = JP(i);
		else if (strcmp(input[i].type, "JN") == 0) i = JN(i);
		else if (strcmp(input[i].type, "L")  == 0)      L(i);
		else if (strcmp(input[i].type, "LA") == 0)     LA(i);
		else if (strcmp(input[i].type, "LR") == 0)     LR(i);
		else if (strcmp(input[i].type, "ST") == 0)     ST(i);
		leftSide(i);
	}
}

int main(void)
{
	strcpy(state, "00");
	strcpy(statePrevious, state);
	maximize();
	hideCursor();
	for (int i = 0; i <= 15; i++) registersNone[i] = -1, registersNonePrevious[i] =- 1, toChange[i] = 1;
	commands = fopen("commands.txt", "r");
	readInput();
	leftSide(-1);
	lineByLine();
	fclose(commands);
	moveTo(max(40, 23 + stackPointer) + 3, 0);
}
