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
void rightSide(int k)
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
	
	//system used to print only 40 lines of code:
	//if code is less than 40 lines, print them all
	if (inputSize <= 40)
	{
		start = 0;
		stop = inputSize;
	}
	
	//if current line number is less than 20
	//print first 40 lines of code
	else if (k <= 20)
	{
		start = 0;
		stop = 40;
	}
	
	//if there are less than 20 lines left,
	//print them all and print more previous lines
	else if (k + 20 > inputSize)
	{
		start = inputSize - 40;
		stop = inputSize;
	}
	
	//in different case print previous and next 20 lines
	else if (20 < k && k + 20 <= inputSize)
	{
		start = k - 20;
		stop = k + 20;
	}
	
	
	
	for (i = start; i < stop; i++)
	{
		lineNumber ++;
		
		//current line, print it with green font
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
			
			//if label was not given, print <NONE> instead
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
			
			//if argument2 was not given, print <NONE> instead
			else printf("<NONE>\n");
			color(0);
		}
		
		//print rest of lines with white font
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
			
			//if label was not given, print <NONE> instead
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
			
			//if argument2 was not given, print <NONE> instead
			else printf("<NONE>\n");
		}
	}
}

//displays left side of terminal
void leftSide(int k)
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
		//if the registrs' value was just changed, print it in red font
		if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i])
		{
			color(3);
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i] == -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			color(0);
		}
		//if the registers' value was not changed and it was printed in red font, re-print it with white font
		else if (toChange[i])
		{
			if (i < 10 && registersNone[i] != -1) clear(), printf("%d:           %d\n", i, registers[i]);
			else if (i < 10 && registersNone[i]== -1) clear(), printf("%d:           NONE\n", i);
			else if (i >= 10 && registersNone[i] != -1) clear(), printf("%d:          %d\n", i, registers[i]);
			else if (i >= 10 && registersNone[i] == -1) clear(), printf("%d:          NONE\n", i);
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
		}
		//else do not do any changes
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
	for (i = 0; i < varNumber; i++)
	{
		line ++;
		//if value was just changed, set its' varNumberPrevious to 1 to remember it
		for (z = memory[i].firstIndex; z <= memory[i].lastIndex; z++)
			if (memoryStackPrevious[z] == 1) 
				varNumberPrevious[i] = 1;
				
		//if variable was just declared, print it in red font
		if (i == k)
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
			
			//if veriable is a single int
			else 
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
			color(0);
		}
		
		//if array's varNumberPrevious equals 1
		//it means there is at least one cell that was meant
		//to print it with red font because some changes was made
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
					//this is the cell that was meant to be printed in red font
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
			
			//if veriable is a single int
			else 
			{
				//if there was a change made, print this value it in red font
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
		
		//value was recently printed in red font, no changes was made
		//so re-print it with white font
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
			
			//if veriable is a single int
			else 
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
		}
		
		//that is the variable that was just declared, so it was printed with red font
		//re-print it with white font
		else if (i == k - 1)
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
			
			//if veriable is a single int
			else 
			{
				printf("%d", firstAddress + memory[i].firstIndex * 4);
				moveTo(27 + line, 13);
				
				printf("%d    ", memoryStack[memory[i].firstIndex]);
				moveTo(27 + line, 26);
				
				printf("%s\n", memory[i].label);
			}
		}
		
		//no changes needs to be made, just skip those lines
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
			//if veriable is a single int
			else 
				printf("\n");
		}
	}
	color(2);
}


//operates GUI
void display(int k)
{
	leftSide(k);
	rightSide(k);
	
	color(2);
	
	//there are no lines of code left
	if (k + 1 == inputSize)
	{
		moveTo(0, 50);
		clear();
		moveTo(0, 80);
		printf("<press enter to close>");
	}
	
	//there are lines of code left 
	else
	{
		moveTo(0, 70);
		printf("<press enter to proceed to next line>");
	} 
	
	color(0);
}
