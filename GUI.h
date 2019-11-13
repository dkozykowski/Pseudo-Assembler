#include <windows.h>
#include <stdio.h>
#define shift 30
#define showContent 1 //if equal to 1 - whole tables' content is leftSideed; if 0 - just indicator to its' first element
#define firstAddress 100

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
	int z = 0;
	for (z = 0; z < 70; z++) printf(" ");
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
	if (k == 0) kolor = 0x000f; //white
	if (k == 1) kolor = 0x000a; //green
	if (k == 2) kolor = 0x000e; //yellow
	if (k == 3) kolor = 0x000c; //red
	HANDLE manual;
	manual = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(manual,kolor);
}

//displays right side of terminal
void rightSide(int *k, int *inputSize, struct singleCommand *input)
{
	int start, stop;
	int lineNumber = 0;
	int i = 0;
	
	color(2);
	moveTo(2, 80 + shift);
	printf("Commands: \n\n");
	moveTo(4, 80 + shift);
	printf("Line nr:   Label:       Type:    Argument1:             Argument2:\n");
	color(0);
	
	if (*inputSize <= 40)
	{
		start = 0;
		stop = *inputSize;
	}
	else if (*k <= 20)
	{
		start = 0;
		stop = 40;
	}
	else if (20 < *k && *k + 20 <= *inputSize)
	{
		start = *k - 20;
		stop = *k + 20;
	}
	else if (*k + 20 > *inputSize)
	{
		start = *inputSize - 40;
		stop = *inputSize;
	}
	
	for (i = start; i < stop; i++)
	{
		lineNumber ++;
		if (i == *k)
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
void leftSide(int *k, int *inputSize, int *varNumber, struct singleCommand *input, int *registers, int *registersPrevious, int *registersNone, int *registersNonePrevious, int *toChange, char state[10], char statePrevious[10], struct variable *memory, int *memoryStack, int *memoryStackPrevious, int *varNumberPrevious)
{
	int i = 0;
	int z = 0;
	int o = 0;
	
	color(2);
	moveTo(2, 0);
	puts("Registers:\n");
	printf("Index:       Value:\n");
	color(0);
	
	for (i = 0; i <= 15; i++)
	{
		if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i])
		{
			color(3);
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i] == -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			color(0);
		}
		else if (toChange[i])
		{
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i]== -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
		}
		else printf("\n");
	}
	color(2);
	printf("\nProgram state: ");
	if (strcmp(state, statePrevious) == 0) color(0);
	else color(3);
	printf("%s", state);
	color(2);
	puts("\n\nMemory:\n");
	puts("Address:     Variable:    Label:");
	color(0);
	int line = -1;
	for (i = 0; i < *varNumber; i++)
	{
		line ++;
		for (z = memory[i].firstIndex; z <= memory[i].lastIndex; z++)
		{
			if (memoryStackPrevious[z] == 1) varNumberPrevious[i] = 1; 
		}
		if (i == *k)
		{ 
			color(3);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d    ", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);

				} 
			}
			
			else //if veriable is a single int
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
			color(0);
		}
		else if (varNumberPrevious[i] == 1)
		{ 
			color(0);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				if (memoryStackPrevious[memory[i].firstIndex]) color(3);
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				color(0);
				
				for (o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					if (memoryStackPrevious[o] == 1) color(3);
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d    ", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);
					color(0);
				} 
			}
			else //if veriable is a single int
			{
				if (memoryStackPrevious[memory[i].firstIndex]) color(3);
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
				color(0);
			}
			color(0);
		}
		else if (varNumberPrevious[i] == -1)
		{ 
			color(0);
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d    ", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);
				} 
			}
			else //if veriable is a single int
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
		}
		else if (i == *k - 1)
		{
			//if veriable is an array
			if (showContent && memory[i].firstIndex + 1 != memory[i].lastIndex)
			{
				
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				
				moveTo(27 + line, 26);
				printf("%s[0]\n", memory[i].label);
				
				for (o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("%d", firstAddress + o * 4);
					
					moveTo(27 + line, 13);
					printf("%d    ", memoryStack[o]);
					
					moveTo(27 + line, 26);
					printf("%s[%d]\n", memory[i].label, z);

				} 
			}
			else //if veriable is a single int
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d    ", memoryStack[memory[i].firstIndex]);
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
				for (o = memory[i].firstIndex + 1, z = 1; o < memory[i].lastIndex; o++, z++)
				{
					line ++;
					printf("\n");
				} 
			}
			else //if veriable is a single int
				printf("\n");
		}
	}
	color(2);
}


//operates GUI
void display(int k, int *inputSize, int *varNumber, struct singleCommand *input, int *registers, int *registersPrevious, int *registersNone, int *registersNonePrevious, int *toChange, char state[10], char statePrevious[10], struct variable *memory, int *memoryStack, int *memoryStackPrevious, int *varNumberPrevious)
{
	leftSide(&k, inputSize, varNumber, input, registers, registersPrevious, registersNone, registersNonePrevious, toChange, state, statePrevious, memory, memoryStack, memoryStackPrevious, varNumberPrevious);
	rightSide(&k, inputSize, input);
	color(2);
	if (k + 1 == *inputSize)
	{
		moveTo(0, 50);
		clear();
		moveTo(0, 80);
		printf("<press enter to close>");
	} 
	else
	{
		moveTo(0, 70);
		printf("<press enter to proceed to next line>");
	} 
	color(0);
	moveTo(0, 0);
}
