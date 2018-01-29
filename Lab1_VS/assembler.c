/*

Name 1: S M Farhanur Rahman
eID 1: sr42946
Name 2: Yaw Turkson
eID 2: yet87

*/


// ---------------- VISUAL STUDIO STUFF ---------------------
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
// ----------------------------------------------------------

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

const char *opcode[] = { "add","and","halt","jmp","jsr","jsrr","ldb","ldw","lea","nop","not","ret","lshf", "rshfl",
"rshfa","rti","stb","stw","trap","xor","brn","brzp","brz","brnp","brp","brnz","br","brnzp" };
#define MAX_LINE_LENGTH 255
enum { DONE, OK, EMPTY_LINE };
FILE* infile = NULL;
FILE* outfile = NULL;

typedef struct Mapping {
	char* label;
	int address;
} Mapping;

typedef struct Table {
	int size;
	int capacity;
	Mapping* mapArray;

} Table;

// -------------- Convert String to Number ---------------------
int toNum(char * pStr) {
	char * t_ptr;
	char * orig_pStr;
	int t_length, k;
	int lNum, lNeg = 0;
	long int lNumLong;

	orig_pStr = pStr;
	if (*pStr == '#')                 /* decimal */
	{
		pStr++;
		if (*pStr == '-')             /* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;

		return lNum;
	}
	else if (*pStr == 'x')        /* hex     */
	{
		pStr++;
		if (*pStr == '-')         /* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
		if (lNeg)
			lNum = -lNum;
		return lNum;
	}
	else
	{
		printf("Error: invalid operand, %s\n", orig_pStr);
		exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
	}
}

// -------------- Parsing Command Line Arguments ---------------------
int parseArgs(int argc, char* argv[]) {

	// if (argc == 3) {	-->	COMMENTED OUT FOR VISUAL STUDIO NEEDS argc = 4

	if (argc == 4) {
		char *prgName = NULL;
		char *iFileName = NULL;
		char *oFileName = NULL;

		/*	COMMENTED OUT FOR VISUAL STUDIO
		prgName = argv[0];
		iFileName = argv[1];
		oFileName = argv[2];
		*/

		prgName = argv[1];
		iFileName = argv[2];
		oFileName = argv[3];

		printf("program name = '%s'\n", prgName);
		printf("input file name = '%s'\n", iFileName);
		printf("output file name = '%s'\n", oFileName);

		return 1;

	}
	else {
		printf("invalid number of inputs\n");
		return 0;
	}
}

// ----------- Parsing Assembly Language ------------------------
int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
) {
	char * lRet, *lPtr;
	int i;
	if (!fgets(pLine, MAX_LINE_LENGTH, pInfile))
		return(DONE);
	for (i = 0; i < strlen(pLine); i++)
		pLine[i] = tolower(pLine[i]);

	/* convert entire line to lowercase */
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	/* ignore the comments */
	lPtr = pLine;

	while (*lPtr != ';' && *lPtr != '\0' &&
		*lPtr != '\n')
		lPtr++;

	*lPtr = '\0';
	if (!(lPtr = strtok(pLine, "\t\n ,")))
		return(EMPTY_LINE);

	if (isOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
	{
		*pLabel = lPtr;
		if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	}

	*pOpcode = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg1 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg2 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg3 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg4 = lPtr;

	return(OK);
}

// ---------------- Check Functions --------------------
int isOpcode(char* text) {
	for (int i = 0; i<28; i++) {
		if (!strcmp(text, opcode[i])) {
			return 1;
		}
	}

	return -1;
}

int isLabel(char * str) {	

	const char *invalidLabel[] = { "in", "out", "getc", "puts" };

	// Length Check
	if (strlen(str) > 20) {
		return 0;
	}

	// First character can't be an x
	if (str[0] == 'x') {
		return 0;
	}

	// Cant be an opcode
	if (isOpcode(str) == 1) {
		return 0;
	}

	// Cant be certain words
	for (int i = 0; i<4; i++) {
		if (!strcmp(str, invalidLabel[i])) {
			return 0;
		}
	}

	// Has to be alpha numeric
	for (int i = 0; i < strlen(str); i++) {
		if (!isalpha(str[i]) && !isdigit(str[i])) {
			return 0;
		}
	}

	return 1;

}

int regCheck(char * lArg1) {

	// Check for valid register
	int num = lArg1[1] - '0';		// Converts number char to int
	if (num > 7) {
		exit(4);
	}

	// Check for letter R
	if (lArg1[0] != 'r') {
		exit(4);
	}

	return 1;

}

int rangeCheck(char * lArg, int bitSize) {

}



// ********************** First Parse Stuff ************************************

struct table* firstParse() {			

	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		*lArg2, *lArg3, *lArg4;

	int lRet;
	int count = 0;
	int startflag = 0;
	int endflag = 0;

	Table* symboltable = (Table*)malloc(sizeof(Table));
	symboltable->size = 0;
	symboltable->capacity = 7;
	symboltable->mapArray = (Mapping*)malloc(7 * sizeof(Mapping));

	/*	
	-> Open the input file <-
	FILE * lInfile;
	lInfile = fopen("data.in", "r");
	*/

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		if (!strcmp(".end", lOpcode) && startflag == 1) {
			endflag = 1;
		}

		if (lRet != DONE && lRet != EMPTY_LINE && startflag == 1 && endflag == 0) {
			
			count++;

			if (strcmp(lLabel, "")) {
				if (isLabel(lLabel) == 0) { exit(1); }
			}

			if (strcmp(lLabel, "") != 0) {
				if (symboltable->size == symboltable->capacity) {
					symboltable->capacity = (symboltable->capacity) * 2;
					realloc(symboltable->mapArray, symboltable->capacity * sizeof(Mapping));
				}

				int i = symboltable->size;
				symboltable->mapArray[i].address = count;
				symboltable->mapArray[i].label = (char*)malloc((strlen(lLabel) + 1) * sizeof(char));
				strcpy(symboltable->mapArray[i].label, lLabel);
				symboltable->size = (symboltable->size) + 1;
				printf("line parsed %i \n", count);

				// For CLion
				fflush(stdout);
				
			}
		}

		if (!strcmp(".orig", lOpcode) && endflag == 0) {
			startflag = 1;

			int num;
			num = toNum(lArg1);
			if (num % 2 != 0) {
				exit(3);
			}
			// printf("0x%.4X\n", num); WORKS
			fprintf(outfile, "0x%.4X\n", num);

		}

	} while (lRet != DONE);

}

// ********************** Second Pass Stuff ************************************

// -------------- Translate all opcodes ------------------------
int Add(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	int value = 0x1000;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement
		int num1 = lArg1[1] - '0';		// Convert register number 
		num1 = num1 << 9;				// Shift and place it in the right position
		value = value | num1;			// Mask it on to the instruction


		// SR1 placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//Check for SR2 or IMME
	if (lArg3[0] == 'r') {
		// Check for valid register
		int num3 = lArg3[1] - '0';		// Converts number char to int
		if (num3 > 7) {
			exit(4);
		}
		else {
			value = value | num3;
			return value;
		}
	}

	else {			

		//!!!!!!! rangeCheck() !!!!!!!!!!

		int num3 = toNum(lArg3);
		if (num3 > 16) {
			exit(3);
		}
		num3 = num3 & 0x001F;			// Cut off the unnecessary parts
		value = value | num3;
		value = value | 32;				// Sets the 5th bit
		return value;
	}
	

}

int And(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	int value = 0x5000;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR1 placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//Check for SR2 or IMME
	if (lArg3[0] == 'r') {
		// Check for valid register
		int num3 = lArg3[1] - '0';		// Converts number char to int
		if (num3 > 7) {
			exit(4);
		}
		else {
			value = value | num3;
			return value;
		}
	}

	else {		

		//!!!!!!! rangeCheck() !!!!!!!!!!

		int num3 = toNum(lArg3);
		if (num3 > 16) {
			exit(3);
		}
		num3 = num3 & 0x001F;		// Cut off the unnecessary parts
		value = value | num3;
		value = value | 32;			// Sets the 5th bit
		return value;
	}
}

void Halt() {

}

void Jmp() {

}

void Jsr() {

}

int Jsrr(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	int value = 0x4000;

	if (regCheck(lArg1)) {
		// BaseR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 6;
		value = value | num1;
		return value;
	}
}

int Ldb(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0x2000;

	if (regCheck(lArg1) && regCheck(lArg2)) {

		// DR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// baseR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;

		//!!!!!!! rangeCheck() !!!!!!!!!!

		// bOffset Placement
		int num3 = toNum(lArg3);
		num3 = num3 & 0x003F;		// Cut off the unnecessary parts
		value = value | num3;			
		return value;
	}

}

int Ldb(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0x6000;

	if (regCheck(lArg1) && regCheck(lArg2)) {

		// DR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// baseR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;

		//!!!!!!! rangeCheck() !!!!!!!!!!

		// Offset Placement
		int num3 = toNum(lArg3);
		num3 = num3 & 0x003F;		// Cut off the unnecessary parts
		value = value | num3;
		return value;
	}

}

void Lea() {

}

void Nop() {

}

int Not(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	int value = 0x903F;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement 
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

}


int Lshf(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0xd000;
	int num;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement 
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//!!!!!!! rangeCheck() !!!!!!!!!!

	num = toNum(lArg3);
	num = num & 0x000f;
	value = value | num;
	return value;

}

int Rshfl(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0xd010;
	int num;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement 
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//!!!!!!! rangeCheck() !!!!!!!!!!

	num = toNum(lArg3);
	num = num & 0x000f;
	value = value | num;
	return value;

}

int Rshfa(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0xd030;
	int num;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement 
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//!!!!!!! rangeCheck() !!!!!!!!!!

	num = toNum(lArg3);
	num = num & 0x000f;
	value = value | num;
	return value;

}


int Stb(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0x3000;

	if (regCheck(lArg1) && regCheck(lArg2)) {

		// SR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// baseR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;

		//!!!!!!! rangeCheck() !!!!!!!!!!

		// Offset Placement
		int num3 = toNum(lArg3);
		num3 = num3 & 0x003F;		// Cut off the unnecessary parts
		value = value | num3;
		return value;
	}

}

int Stw(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {

	int value = 0x7000;

	if (regCheck(lArg1) && regCheck(lArg2)) {

		// SR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// baseR placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;

		//!!!!!!! rangeCheck() !!!!!!!!!!

		// Offset Placement
		int num3 = toNum(lArg3);
		num3 = num3 & 0x003F;		// Cut off the unnecessary parts
		value = value | num3;
		return value;
	}

}

void Trap() {

}

void Xor(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	int value = 0x8000;

	if (regCheck(lArg1) && regCheck(lArg2)) {
		// DR placement
		int num1 = lArg1[1] - '0';
		num1 = num1 << 9;
		value = value | num1;

		// SR1 placement
		int num2 = lArg2[1] - '0';
		num2 = num2 << 6;
		value = value | num2;
	}

	//Check for SR2 or IMME
	if (lArg3[0] == 'r') {
		// Check for valid register
		int num3 = lArg3[1] - '0';		// Converts number char to int
		if (num3 > 7) {
			exit(4);
		}
		else {
			value = value | num3;
			return value;
		}
	}

	else {		

		//!!!!!!! rangeCheck() !!!!!!!!!!

		int num3 = toNum(lArg3);
		if (num3 > 16) {
			exit(3);
		}
		num3 = num3 & 0x001F;		// Cut off the unnecessary parts
		value = value | num3;
		value = value | 32;			// Sets the 5th bit
		return value;
	}
}

void Brn() {

}

void Brzp() {

}

void Brz() {

}

void Brnp() {

}

void Brp() {

}

void Brnz() {

}

void Br() {

}

void Brnzp() {

}

void secondParse() {

	// Call ReadAndParse()  
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		*lArg2, *lArg3, *lArg4;

	int lRet;

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if (lRet != DONE && lRet != EMPTY_LINE)
		{

			// -------------- Identify opcodes ------------------------
			
			//The trap vector for a TRAP instruction and the shift amount for SHF instructions must be non-negative values. 
		
			if (!strcmp(lOpcode, opcode[0])) {

				/*	ADD()	 */

				int num = Add(lArg1,lArg2,lArg3,lArg4);
				printf("0x%.4X\n", num); 

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[1])) {

				/* AND() */

				int num = And(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[2])) {

				/*	Halt()  */

				int num;
				int value = 0xf025;	// Trapx25
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);


			}

			else if (!strcmp(lOpcode, opcode[3])) {

				/*	Jmp() */

				int value = 0xB000;
				if (regCheck(lArg1)) {
					// Convert register number 
					int num1 = lArg1[1] - '0';

					// Shift and place it in the right position
					num1 = num1 << 6;

					// Mask it on to the instruction
					value = value | num1;

					printf("0x%.4X\n", value);

					// File Output:
					//fprintf(outfile, "0x%.4X\n", num);
				}
				else {
					exit(3);
				}

			}

			else if (!strcmp(lOpcode, opcode[4])) {
				Jsr();
			}

			else if (!strcmp(lOpcode, opcode[5])) {
				
				/* JSRR() */

				int num = Jsrr(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[6])) {
				/* Ldb(); */

				int num = Ldb(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[7])) {
				
				/* Ldw(); */

				int num = Ldw(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[8])) {
				Lea();
			}

			else if (!strcmp(lOpcode, opcode[9])) {
				Nop();
			}

			else if (!strcmp(lOpcode, opcode[10])) {
				
				/*	Not()  */

				int num = Add(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[11])) {
				
				/*	Ret()  */

				int value = 0xB1B0;
				printf("0x%.4X\n", value);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);


			}

			else if (!strcmp(lOpcode, opcode[12])) {
				
				/*	Lshf()  */
				

				int num = Lshf(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[13])) {
				
				/*	Rshf()  */

				int num = Rshf(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			
			}

			else if (!strcmp(lOpcode, opcode[14])) {

				/*	Rsha()  */

				int num = Rsha(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[15])) {
				
				/*	Rti()  */

				int value = 0x8000;
				printf("0x%.4X\n", value);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);


			}

			else if (!strcmp(lOpcode, opcode[16])) {

				/* Stb(); */

				int num = stb(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[17])) {
				/* Stw(); */

				int num = stw(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[18])) {

				/*	Trap()  */

				int num;
				int value = 0xf000;

				// Can't have negative constants
				if (lArg1[1] == '-') {
					exit(3);
				}
				else {
					num = toNum(lArg1);
					num = num & 0x00ff;
					value = value | num;

					printf("0x%.4X\n", num);

					// File Output:
					//fprintf(outfile, "0x%.4X\n", num);

				}
			}

			else if (!strcmp(lOpcode, opcode[19])) {
				
				/*	Xor()  */

				int num = xOR(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			else if (!strcmp(lOpcode, opcode[20])) {
				Brn();
			}

			else if (!strcmp(lOpcode, opcode[21])) {
				Brzp();
			}

			else if (!strcmp(lOpcode, opcode[22])) {
				Brz();
			}

			if (!strcmp(lOpcode, opcode[23])) {
				Brnp();
			}

			else if (!strcmp(lOpcode, opcode[24])) {
				Brp();
			}

			else if (!strcmp(lOpcode, opcode[25])) {
				Brnz();
			}

			else if (!strcmp(lOpcode, opcode[26])) {
				Br();
			}

			else if (!strcmp(lOpcode, opcode[27])) {
				Brnzp();
			}

			// File output

		}
	} while (lRet != DONE);

}

// ********************** MAIN ************************************
int main(int argc, char* argv[]) {
	if (parseArgs(argc, argv)) {
		/* open the source file */

		/*	COMMENTED OUT FOR VISUAL STUDIO
		infile = fopen(argv[1], "r");
		outfile = fopen(argv[2], "w");
		*/

		infile = fopen(argv[2], "r");
		outfile = fopen(argv[3], "w");

		if (!infile) {
			printf("Error: Cannot open file %s\n", argv[1]);
			exit(4);
		}
		if (!outfile) {
			printf("Error: Cannot open file %s\n", argv[2]);
			exit(4);
		}

		/* Do stuff with files */

		firstParse();

		infile = fopen(argv[2], "r");
		outfile = fopen(argv[3], "w");
		secondParse();


		printf("successfully did things with files\n");
		fclose(infile);
		fclose(outfile);

	}


}
