//updates color of old variables from red to white
void update(int k)
{
	int i = 0;
	int o = 0;
	
	for (i = 0; i <= 15; i++)
	{
		//if there was a change made, save it in toChange to print it in red font later
		if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i])
		{
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 1;
		}
		
		//if there was no change made, just update all arrays
		else if (toChange[i])
		{
			if (registersNonePrevious[i] != registersNone[i] || registers[i] != registersPrevious[i]) color(0);
			registersNonePrevious[i] = registersNone[i];
			registersPrevious[i] = registers[i];
			toChange[i] = 0;
		}
	}
	
	//update old status
	strcpy(statePrevious, state);
	
	for (i = 0; i < varNumber; i++)
	{
		
		//if at least one of arrays' cell was changed, save it
		for (o = memory[i].firstIndex; o < memory[i].lastIndex; o++)
			if (memoryStackPrevious[o] == 1) varPrevious[i] = 1;
		
		//if variable or some of array's cell was printed with red font, save it in varPrevious
		//to print it in white font later
		if (varPrevious[i] == 1)
		{ 
			varPrevious[i] = -1;
		
			for (o = memory[i].firstIndex; o < memory[i].lastIndex; o++)
				if (memoryStackPrevious[o] == 1) memoryStackPrevious[o] = -1;
		}

		//if variable or some of array's cell was re-printed with white font, 
		//update all arrays to remember it needs re-printing anymore
		else if (varPrevious[i] == -1)
		{ 
			varPrevious[i] = 0;
		
			for (o = memory[i].firstIndex; o < memory[i].lastIndex; o++)
				if (memoryStackPrevious[o] == -1) memoryStackPrevious[o] = 0;		
		}
	}
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

//<label> DC <arg1> <arg2>
//allocates memory with value
void DC(int i)
{	
	int negative = 1;
	int howMuch1 = 0;
	int o;
	int power10 = 1;
	int howMuch = 0;
	int z = strlen(input[i].argument1);
	
	strcpy(memory[varNumber].label, input[i].label);
	memory[varNumber].firstIndex = stackPointer;
	
	//get the value
	if (input[i].argument1[z - 1] == ')')
	{
		for (o = z - 2; input[i].argument1[o] != '('; o--)
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
	
	//get the ammount of cells
	for (o = 0; input[i].argument1[o] != '*'; o++)
	{
		if (!('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')) break;
		howMuch *= 10;
		howMuch += input[i].argument1[o] - '0';
	}
	
	howMuch = max(1, howMuch);
	howMuch += stackPointer;
	
	//get the shift of address
	for (; stackPointer < howMuch; (stackPointer)++) memoryStack[stackPointer] = negative * howMuch1;
	
	memory[varNumber].lastIndex = stackPointer;
	varNumber++;
}

//<label> DS <arg1>
//allocates memory without value
void DS(int i)
{
	int o;
	int howMuch = 0;
	
	strcpy(memory[varNumber].label, input[i].label);
	memory[varNumber].firstIndex = stackPointer;
	
	//get the ammount of cells
	if (input[i].argument1[0] != 'I')
	{
		for (o = 0; input[i].argument1[o] != '*'; o++)
		{
			howMuch *= 10;
			howMuch += input[i].argument1[o] - '0';
		}
		stackPointer += howMuch;
	}
	//if the variable is just a single int
	else (stackPointer)++;
	
	memory[varNumber].lastIndex = stackPointer;
	(varNumber)++;
}

//<label> A <arg1> <arg2>
//adds memory with index arg2 to register with index arg2
void A(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	int o;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0; 
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
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
	int o = 0;
	char x = 0;	
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0;
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
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
	int o = 0;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0;
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
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
	int o = 0;
	char x = 0;	
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0;
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
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
	int o = 0;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0;
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
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
	int k = 0;
	
	for (k = 0; k < inputSize; k++)
		if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i + 1;
}

//<label> JZ arg1
//if program state == 00: jumps to line with label == arg1
int JZ(int i)
{
	int k = 0;
	
	if (strcmp(state, "00") == 0)
		for (k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i + 1;
}

//<label> JP arg1
//if program state == 01: jumps to line with label == arg1
int JP(int i)
{
	int k = 0;
	
	if (strcmp(state, "01") == 0)
		for (k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i + 1;
}

//<label> JN arg1
//if program state == 10: jumps to line with label == arg1
int JN(int i)
{
	int k = 0;
	
	if (strcmp(state, "10") == 0)
		for (k = 0; k < inputSize; k++)
			if (strcmp(input[k].label, input[i].argument1) == 0) return k;
	return i + 1;
}

//<label> L <arg1> <arg2>
//loads memory with index arg2 value to register with index arg1
void L(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	int o = 0;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument's address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = memoryStack[registers[howMuch] / 4 + index2];
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
	int o = 0;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
		index2 -= firstAddress;
		index2 /= 4;
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
			if (strcmp(memory[o].label, temporaryLabel) == 0) 
				index2 = memory[o].firstIndex;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = registers[howMuch] / 4 + index2;
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
	int o = 0;
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument2[o] && input[i].argument2[o] <= '9')
		{
			index2 *= 10;
			index2 += input[i].argument2[o] - '0';
		}
		else break;
	}
	registers[index] = registers[index2];
	registersNone[index] = 0;
}

//<label> ST <arg1> <arg2>
//loads register with index arg1 value to memory with index arg2
void ST(int i)
{
	char temporaryLabel[30];
	int index = 0;
	int index2 = 0;
	int z = 0;
	int o = 0;
	char x = 0;
	int k = strlen(input[i].argument2);
	int howMuch = 0;
	
	//get the first argument address
	for (o = 0; o < 10; o++)
	{
		if ('0' <= input[i].argument1[o] && input[i].argument1[o] <= '9')
		{
			index *= 10;
			index += input[i].argument1[o] - '0';
		}
		else break;
	}
	
	//get the shift of addresses
	if ('0' <= input[i].argument2[0] && input[i].argument2[0] <= '9')
	{
		for (; (z < k && input[i].argument2[z] != '('); z++)
		{
			index2 *= 10;
			index2 += input[i].argument2[z] - '0';
		}
	}
	
	//change label to address
	else
	{	
		for (z = 0; z < 30; z++) temporaryLabel[z] = x;
		for (z = 0; z < k; z++)
		{
			if (input[i].argument2[z] == '(') break;
			temporaryLabel[z] = input[i].argument2[z];
		}
		for (o = 0; o < varNumber; o++)
		if (strcmp(memory[o].label, temporaryLabel) == 0)
		{ 
			index2 = memory[o].firstIndex;
			varPrevious[o] = 1;
		}
		index2 = index2 * 4 + 100;										
	}
	
	//get the register number if address was shifted
	z ++;
	for (; z < k - 1; z++)
	{
		howMuch *= 10;
		howMuch += input[i].argument2[z] - '0';
	}
	
	if(input[i].argument2[k - 1] == ')') index2 = (registers[howMuch] + index2 - 100) /4;
	else index2 = (index2 - 100) / 4;
	memoryStackPrevious[index2] = 1;
	memoryStack[index2] = registers[index];
}

//calls function to operate every single line of input in terminal one by one
void lineByLine()
{
	int i;
	char x[1000];
	int z;
	int o;
	int j;
	
	for (i = 0; i < inputSize; i++)
	{
		z = i + 1;
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
		else if (strcmp(input[i].type, "J")  == 0)  z = J(i);
		else if (strcmp(input[i].type, "JZ") == 0) z = JZ(i);
		else if (strcmp(input[i].type, "JP") == 0) z = JP(i);
		else if (strcmp(input[i].type, "JN") == 0) z = JN(i);
		else if (strcmp(input[i].type, "L")  == 0)      L(i);
		else if (strcmp(input[i].type, "LA") == 0)     LA(i);
		else if (strcmp(input[i].type, "LR") == 0)     LR(i);
		else if (strcmp(input[i].type, "ST") == 0)     ST(i);
		
		//if value was just changed, set its' varPrevious to 1 to remember it
		for (o = 0; o < varNumber; o++)	
			for (j = memory[o].firstIndex; j < memory[o].lastIndex; j++)
				if (memoryStackPrevious[j] == 1) 
					varPrevious[o] = 1;
		display(i);
		update(i);
		i = z - 1;
		
		gets(x);
	}
}

//reads given file line by line and creates an array of struct commands
void readInput()
{
	char x[100];
	char inputLine[1000];
	int argIndex = 0; 
	int charIndex = 0; 
	int i = 0;
	
	color(0);
	printf("Please write Your program name with extention: ");
	scanf("%s", fileName);
	commands = fopen(fileName, "r");
	system("cls");
	
	while(fgets(inputLine, 1000, commands) != NULL)
	{
		if (inputLine[0] == '\n') continue;
		argIndex = charIndex = 0;
		
		for (i = 0; (i < (int)strlen(inputLine) && argIndex < 4); i++)
		{
			if (('A' <= inputLine[i] && inputLine[i] <= 'Z') || ('0' <= inputLine[i] && inputLine[i] <= '9') || inputLine[i] == '-' || inputLine[i] == '*' || inputLine[i] == '(' || inputLine[i] == ')' || inputLine[i] == '_')
			{
				//label
				if (argIndex == 0)
					input[inputSize].label[charIndex] = inputLine[i];
					
				//type
				if (argIndex == 1)
					input[inputSize].type[charIndex] = inputLine[i];
					
				//argument1
				if (argIndex == 2)
					input[inputSize].argument1[charIndex] = inputLine[i];
					
				//argument2
				if (argIndex == 3)
					input[inputSize].argument2[charIndex] = inputLine[i];
					
				charIndex++;
			}
				
			//move to next element of the command
			else if (charIndex > 0)
			{
				argIndex++;
				charIndex = 0;
			}
			
			//if the given label is a name reserved to commands type
			//then it is actually command type and label was not given 
			if (argIndex == 1 && check(inputSize))
			{
				strcpy(input[inputSize].type, input[inputSize].label);
				strcpy(input[inputSize].label, "");
				argIndex++;
			}
		}
		(inputSize)++;
	}
	fclose(commands);
	
	gets(x);
	
	lineByLine();
}
