//  name: Liam Hare
// project title: Assembler Project
// date: 2/6/2016
// i/o files: Assembly File --> Assembler1LH.asm
// description: This is a simple assembler that reads in assembly code from a file, converts it to machine code, then runs it. 

#define _CRT_SECURE_NO_WARNINGS  // lets us use deprecated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

char ASM_FILE_NAME[] = "Assembler1LH.asm";

#define MAX 150			// strlen of simulators memory can be changed
#define COL 9			// number of columns for output
#define LINE_SIZE 20	// For c-strings

//OPERAND TYPES, REGISTERS AND OTHER
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define ADDRESS 6
#define CONSTANT 7

//commands
#define HALT 5
#define PUT 7
#define ADD 160
#define MOVREG 192
#define MOVMEM 224

//boolean
#define TRUE 1
#define FALSE 0

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types

//Registers and flag add the stack pointer
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
}regis;

//GLOBAL VARIABLES
typedef short int Memory;  //sets the type of memory to short int 
Memory memory[MAX] = { 0 };   //global variable the memory of the virtual machine
Memory address;     //global variable the current address in the virtual machine
Memory stackPointer;  //globla variable with the current address of the stack

//comments with NEEDS WORK means the function and/or the comment needs work/correcting.  Remove ***NEEDS WORK*** comment once you 
//         have read the comment and started the process
//function prototypes for functions that will be changed, the function name may not be changed ***NEEDS WORK***
void runMachineCode();	// Executes the machine code	 ****NEEDS WORK***
void splitCommand(char line[], char part1[], char part2[], char part3[]);	// splits line of asm into it's three parts	****NEEDS WORK***
void convertToMachineCode(FILE* fin);	// Converts a single line of ASM to machine code	***NEEDS WORK***
void assembler();			// Converts the entire ASM file and stores it in memory
void runMachineCode();	// Executes the machine code	
void putValue(int operand, int value);  //prototype only ***NEEDS WORK***
Memory getValue(Memory operand); //prototype only ***NEEDS WORK***

// Helper functions,this functions may be changed ****NEEDS WORK***
int changeToNumber(char line[], int start);	// converts a sub-string to an int, 
//may change function so start is passed by reference
//so it is the location directly after the number ***NEEDS WORK***

// Helper functions prototypes, these functions should NOT be changed ****NEEDS WORK***
int whichOperand(char operand[]);			// Returns the number of the registar or other operand
void changeToLowerCase(char line[]);	// Changes each character in the string to lower case
void printMemoryDump();	               // Prints memeory with commands represented as integes
void printMemoryDumpHex();				   // Prints memory in hexedecimal
int isDigitOrNeg(char letter);            // is a charater the start of a positive or negative number
void registerStartValues();              // gives all registers & flag random values to start

//***needs work ***
/********************   getValue   ********************
Function to

parameters:
	Memory operand -
	int value -


return values:

-------------------------------------------------------*/
Memory getValue(Memory operand)
{
	int value1;
	if (operand == AXREG)
	{
		return regis.AX;
	}
	if (operand == BXREG)
	{
		return regis.BX;
	}
	if (operand == CXREG)
	{
		return regis.CX;
	}
	if (operand == DXREG)
	{
		return regis.DX;
	}
	else if (operand == CONSTANT)
	{
		value1 = memory[address];
		address++;
		return value1;
	}
}

/********************   putValue   ********************
Function to

parameters:
	Memory operand -  
	int value - 
	

return values:
	
-------------------------------------------------------*/
void putValue(Memory operand, int value)
{
	if (operand == AXREG)
	{
		regis.AX = value;
	}
	if (operand == BXREG)
	{
		regis.BX = value;
	}
	if (operand == CXREG)
	{
		regis.CX = value;
	}
	if (operand == DXREG)
	{
		regis.DX = value;
	}
	else if (operand == MOVMEM)
	{
		int newAddr = memory[address];
		address++;
		memory[newAddr] = value;
	}
}
//no comment needed for function main, its comment is the header for the project
int main()
{
	assembler();
	printf("\n\nConvert to Memory complete\n");
	printMemoryDump();      //may also have printMemoryDumpHex but you must have printMemoryDump

	runMachineCode();
	printf("\n\nFinal Memory dump in integers\n");
	printMemoryDump();  //displays memory with final values
	printf("\n\nFinal Memory dump in hex\n");
	printMemoryDumpHex();  //displays memory with final values in hex 

	printf("\n");   //puts a blank line betwwen the code output and the system output
	return 0;
}

/********************   assembler   ***********************
* changes the assembly code to machine code and places the
*         commands into the memory.
*parameters: none
*return value: none
-----------------------------------------------------------*/
void assembler()
{
	registerStartValues();
	address = 0;

	FILE* fin;		// File pointer for reading in the assembly code.
	fopen_s(&fin, ASM_FILE_NAME, "r");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
	}
	for (int i = 0; i < MAX && !feof(fin); i++)
	{
		convertToMachineCode(fin);
		//output memory, for debugging, comment out when you don't need it. Could use printMemoryDumpHex
		printMemoryDump();  //debugging.
	}
}

/********************   convertToMachineCode   ***********************
* Converts a single line of ASM to machine code

***Needs work***, comment must be corrected
---------------------------------------------------------------------*/
void convertToMachineCode(FILE* fin)
{
	char line[LINE_SIZE]; // full command
	char part1[LINE_SIZE]; //the asm commmand
	char part2[LINE_SIZE] = ""; // the two operands, could be empty
	char part3[LINE_SIZE] = "";
	int machineCode = 0;			//one line of converted asm code from the file

	fgets(line, LINE_SIZE, fin);		//takes one line from the asm file
	changeToLowerCase(line);	// COMMENT

	splitCommand(line, part1, part2, part3);// split larger commands

	// ------------------ Empty Lines ------------------
		//(Do we need something to handle empty lines or numbers?)
	
	// --------------- One Part Commands ---------------
	if (line[0] == 'h')
	{
		memory[address] = HALT;
		address++;
		return;
	}
	else if (line[0] == 'p')
	{
		machineCode = PUT;
		memory[address] = machineCode;
		address++;
		return;
	}

	// --------------- Two Part Commands  ---------------

	// --------------- Three Part Commands  ---------------
	else
	{
		splitCommand( line, part1, part2, part3 );
		printf( "\nCommand = P1=%s P2=%s P3=%s", part1, part2, part3);

		if (part1[0] == 'm')  //if first character of part1 is [m]
		{
			if (part2[0] == '[')
			{
				machineCode = MOVMEM;
				char temp[LINE_SIZE];
				strcpy( temp, part3);
				strcpy( part3, part2);
				strcpy( part2, temp);
			}
			else
			{
				machineCode = MOVREG;
			}
		}
		else if (part1[0] == 'a')	//if first character of part1 is [a] (Insert add functionality)
		{
			machineCode = ADD; 
		}

		int operand1 = whichOperand( part2 ) << 3;
		int operand2 = whichOperand(part3);

		machineCode = machineCode + operand1 + operand2;
		memory[address] = machineCode;
		address++;

		if (operand2 == CONSTANT || operand2 == ADDRESS)
		{
			memory[address] = changeToNumber( part3, 0);
			address++;
		}
} //end convertToMachineCode


/********************   runMachineCode   ***********************
Executes the machine code that is in memory, the virtual machine

Comment Needs to be written
-----------------------------------------------------------*/
void runMachineCode()
{
	Memory = MASK1 224  //111 00 000
	Memory MASK2 24    //000 11 000
	Memory = MASK3 7	  //000 00 111

	Memory part1, part2, part3; // the three parts of the line
	int value1, value2;   //the actual values in the registers or constants

	address = 0;  //reset address to the first item in memory
	Memory fullCommand = memory[address];  //not yet broken into the three parts
	address++;
		while (fullCommand != HALT)
		{
			part1 = fullCommand & MASK1;
			part2 = (fullCommand & MASK2) >> 3;
			part3 = fullCommand & MASK3;
		
			if ( fullCommand == PUT)
			{
				printf("\t AX is %d\n", regis.AX);
			}

			else if (part1 == MOVREG)
			{
				value1 = getValue( part3 );
				//printf("Value is %d \n", value1);
				putValue( part2, value1 );
			}
		
			else if (part1 == ADD)
			{
				value1 = getValue( part3 );
				value2 = getValue(part2);
				value1 = value1 + value2;
				putValue( part2, value1);
			}

			else if ( part1 == MOVMEM)
			{
				value1 = getValue(part2);
				putValue( part2, value1);
			}

			else if (address > 0) //debugging tool change address to show what you are unsure about
			{
				printMemoryDump(); //debugging, comment out at end of semester 
			}
			fullCommand = memory[address];
			address++;
		}//end of loop
	} //end runMachineCode

/*********************************************************************************
/****************************   HELPER FUNCTIONS   *******************************
/*** *** *** Given with starting code may not be changed nor rewritten *** *** ***
/********* Unless it is indicated in the function header comment
/*********************************************************************************

/*****************************************************************************/
/*********** helper function for converting to machine code ******************/
/*****************************************************************************/

/********************   splitCommand   ***********************
Splits a line of asm into it's parts

Takes in a line of assembly code and splits it into three separate pieces.

parameters:
	char line[] - entire line of assembly code
	char part1[] - the command
	char part2[] - the first operand
	char part3[] - the second operand

return values:
	nothing
-----------------------------------------------------------*/
void splitCommand(char line[], char part1[], char part2[], char part3[])
{
	int index = 0;           //the character location in the string line
	int partIndex = 0;          //character location in the line of each string (or part)
	
	// set all strings to empty, in case of empty or shorter operands
	part1[0] = '\0';
	part2[0] = '\0';
	part3[0] = '\0';

	// --------------- First ---------------
	
	//moves the first set of characters from line into part1
	while (line[index] != ' ' && line[index] != '\0' && line[index] != '\n')
	{
		part1[partIndex] = line[index];
		index++;
		partIndex++;
	}
	part1[partIndex] = '\0'; // adds the string stopper character
	
	index++;
	partIndex = 0;

	// --------------- Second Part ---------------

	//moves the first set of characters from line into part1
	while (line[index] != ' ' && line[index] != '\0' && line[index] != '\n')
	{
		part2[partIndex] = line[index];
		index++;
		partIndex++;
	}
	part2[partIndex] = '\0'; // adds the string stopper character

	printf("part1%s, part2%s", part1, part2);
	if (line [index == '0'] || line [index] == '\n')
	{
		part3[0] = '\0';
	}

	// --------------- Three Part Commands ---------------
	else
	{
		index++;
		partIndex = 0;

		while (line [index] != ' ' && line [index] != '\0' && line [index] != '\n')
		{
			part3[partIndex] = line[index];
			index++;
			partIndex++;
		}
		part3[partIndex] = '\0'; // adds the string stopper character

		if (line[index] != '0')
		{
			printf("\nlineIndex is: %d", index);
		}
} //end splitCommand

/*********************   whichOperand   ***************************
* Determines which operand is being referenced and returns the appropriate letter
*		0-3 registers
*		4 bx addrsss - added in part 7
*		5 bx address plus offset -added in part 7
*		6 address -added in part 5
*		7 constant
*parameters:
* letter - the first letter of the operand, register, number, [
*return value - the number of the register
--------------------------------------------------------------*/
int whichOperand(char operand[LINE_SIZE])
{
	char letter = operand[0];
	if (letter == 'a')
	{
		return AXREG;
	}
	else if (letter == 'b')
	{
		return BXREG;
	}
	else if (letter == 'c')
	{
		return CXREG;
	}
	else if (letter == 'd')
	{
		return DXREG;
	}
	else if (isDigitOrNeg(letter))
	{
		return CONSTANT;
	}
	return -1;  //something went wrong if -1 is returned
}//end whichOperand

/*********************ChangeToNumber ********************
* takes in a line and converts digits to a integer
*parameters:
*	line - is the string of assembly code to convert
*  start - is the location where the line is being converted
*return value:
*  the integer value of the string
* ***needs work*** may be changed so that the start is passed  by reference
*--------------------------------------------------------------*/
int changeToNumber(char line[], int start)
{
	int value; // is the integer value of the digits in the code
	char number[16];  //just the digits
	int negative = 0;  //negative or positive number

	int i = 0;
	while (line[start] == '[' || line[start] == ' ')
	{
		start++;
	}
	if (line[start] == '-')
	{
		start++;
		negative = 1;
	}
	while (i < 16 && isdigit(line[start]))
	{
		number[i] = line[start];
		i++;
		start++;
	}
	number[i] = '\0';
	value = atoi(number);
	if (negative == 1)
	{
		value = -value;
	}
	return value;
}  //end convertToNumber

/*********************   changeToLowerCase   ********************
* Changes each character in the line to lower case
*parameters:
* line - the string that was entered the line is completely changed to lower case
*return value: none
----------------------------------------------------------------*/
void changeToLowerCase(char line[])
{
	int index = 0;
	while (index < strlen(line))
	{
		line[index] = tolower(line[index]);
		index++;
	}
} //end changeToLowerCase

/*********************   isDigitOrNeg   ********************
* Determines if a character is the start of a number positive or negative
*      true (1): if the character is a digit or a negative sign
*      false (0): otherwise
*parameters:
* letter - a character
*return value: if the character is a number or not
----------------------------------------------------------------*/
int isDigitOrNeg(char letter)
{
	int retValue = 0;
	if (letter == '-' || isdigit(letter))
	{
		retValue = 1;
	}
	return retValue;
}  //end isDigitNeg

/*********************  registerStartValues  *****************************
* gives all registers and the flag random values to start the code.
*    This is realistic because unless the machine is just been turned on
*    the values will be unknown.
*    The values are upto 4 digits. The formula for AX = (high - low + 1)+low;
*			high = 9999
*			low = 2
*parameters: none
*return value: none
--------------------------------------------------------------------*/
void registerStartValues()
{
	srand(0);  //starts random number generator, will always have the same values, best for debugging
	//srand(time(NULL) ); //starts random generator with a the time, will appear more random
	regis.AX = rand() % (9999 - 2 + 1) + 2;
	regis.BX = rand() % 9998 + 2;
	regis.CX = rand() % 9998 + 2;
	regis.DX = rand() % 9998 + 2;
	regis.flag = rand() % 9998 + 2;
} //end of registerStartValues

/*********************************************************************************
/***********************   HELPER FUNCTIONS for debugging  ***********************
/*********************************************************************************

/****************************   printMemoryDump   ********************************
* prints memory out as integers
*     MAX (constant) is the amount of elements in the memory memory array
*     COL (constant) is the number of columns that are to be displayed
* parameters: none
* return value: none
---------------------------------------------------------------------------------*/
void ()
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	printf("\n");
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				printf("%5d.%5d", location, memory[location]);
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf("\n");
	}
	printf("\nAX: %d", regis.AX);
	printf("%7s: %d", "BX", regis.BX);
	printf("%7s: %d", "CX", regis.CX);
	printf("%7s: %d", "DX", regis.DX);
	printf("%9s: %d\n", "flag", regis.flag);
	printf("Address: %d\t", address);
	printf("Stack Pointer:%d\n\n", stackPointer);
}//end printMemoryDump

/*********************   printMemoryDumpHex   ********************
* Prints memory in hexidecimal
*     MAX is the amount of elements in the memory memory array
*     COL is the number of columns that are to be displayed
* parameters: none
* return value: none
----------------------------------------------------------------*/
void printMemoryDumpHex()
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	printf("\n");
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				printf("%5d.%3x", location, memory[location]);
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf("\n");
	}
	printf("\nAX: %d", regis.AX);
	printf("%7s: %d", "BX", regis.BX);
	printf("%7s: %d", "CX", regis.CX);
	printf("%7s: %d", "DX", regis.DX);
	printf("%9s: %d\n", "flag", regis.flag);
	printf("Address: %d\t", address);
	printf("Stack Pointer:%d\n\n", stackPointer);
} //end printMemoryDumpHex


/*Problems:
*	As you encounter problems "head bangers" list them here with your solution if you
*	found one.  Remove this with your final solution at the end of the semester. If you
*	didn't have major issues write "no problems."  If the code doesn't work and you have
*  "no Problems" you will lose points.
* Part 1 -  2/9 -  split function is working incorrectly. Some logic is definitely switched up but I shoule be able to solve it.
*			2/15 - No Problems.
* Part 2 -  2/9 - As a whole has issues, need to figure out the bit-shifting piece.
* Part 3 - 
*/

/*Notes/Fixes

* Part 1 -  2/9 -  split function is working incorrectly. Some logic is definitely switched up but I shoule be able to solve it.
*			2/15 - split function seems to have been fixed. Instead of using an if/else statement to move between part1, part2,
*			& part3, I am using if statements to make sure there are more characters in the string, if not, returning.
* Part 2 -  
* Part 3 -
*/

