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

void drCheck() {

}

void srCheck() {

}

void ImmeCheck() {

}


// ********************** First Parse Stuff ************************************

void firstParse() {			

	// Call ReadAndParse()  
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		*lArg2, *lArg3, *lArg4;

	int lRet;

	/*	
	-> Open the input file <-
	FILE * lInfile;
	lInfile = fopen("data.in", "r");
	*/

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if (lRet != DONE && lRet != EMPTY_LINE)
		{

			//count++;

			if (strcmp(lLabel, "")) {
				if (isLabel(lLabel) == 0) { exit(1); }
			}

			if (strcmp(lLabel, "") != 0) {
				/*
				if ((*symboltable).size == (*symboltable).capacity) {
					(*symboltable).capacity = ((*symboltable).capacity) * 2;
					realloc((*symboltable).symbolarr, (*symboltable).capacity * sizeof(struct symbol));
				}
				(*symboltable).symbolarr[(*symboltable).size]->address = count;
				(*symboltable).symbolarr[(*symboltable).size]->label = (char *)malloc((strlen(lLabel) + 1) * sizeof(char));
				strcpy((*symboltable).symbolarr[(*symboltable).size]->label, lLabel);
				(*symboltable).size++;
				printf("line parsed %i \n", count);
				fflush(stdout);
				*/
			}
		}
		
	} while (lRet != DONE);

}

// ********************** Second Pass Stuff ************************************

// -------------- Translate all opcodes ------------------------
void Add(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
	//DR check

	//SR1 check 

	//SR2 check

	//Imme check

	fprintf(outfile, "0x1%s%s%s");

}

void And() {

}

void Halt() {

}

void Jmp() {

}

void Jsr() {

}

void Jsrr() {

}

void Ldb() {

}

void Ldw() {

}

void Lea() {

}

void Nop() {

}

void Not() {

}

void Ret() {

}

void Lshf() {

}

void Rshfl() {

}

void Rshfa() {

}

void Rti() {

}

void Stb() {

}

void Stw() {

}

void Trap() {

}

void Xor() {

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
			
		
			if (!strcmp(lOpcode, opcode[0])) {
				Add(&lArg1,&lArg2,&lArg3,&lArg4);
			}

			if (!strcmp(lOpcode, opcode[1])) {
				And();
			}

			if (!strcmp(lOpcode, opcode[2])) {
				Halt();
			}

			if (!strcmp(lOpcode, opcode[3])) {
				Jmp();
			}

			if (!strcmp(lOpcode, opcode[4])) {
				Jsr();
			}

			if (!strcmp(lOpcode, opcode[5])) {
				Jsrr();
			}

			if (!strcmp(lOpcode, opcode[6])) {
				Ldb();
			}

			if (!strcmp(lOpcode, opcode[7])) {
				Ldw();
			}

			if (!strcmp(lOpcode, opcode[8])) {
				Lea();
			}

			if (!strcmp(lOpcode, opcode[9])) {
				Nop();
			}

			if (!strcmp(lOpcode, opcode[10])) {
				Not();
			}

			if (!strcmp(lOpcode, opcode[11])) {
				Ret();
			}

			if (!strcmp(lOpcode, opcode[12])) {
				Lshf();
			}

			if (!strcmp(lOpcode, opcode[13])) {
				Rshfl();
			}

			if (!strcmp(lOpcode, opcode[14])) {
				Rshfa();
			}

			if (!strcmp(lOpcode, opcode[15])) {
				Rti();
			}

			if (!strcmp(lOpcode, opcode[16])) {
				Stb();
			}

			if (!strcmp(lOpcode, opcode[17])) {
				Stw();
			}

			if (!strcmp(lOpcode, opcode[18])) {
				Trap();
			}

			if (!strcmp(lOpcode, opcode[19])) {
				Xor();
			}

			if (!strcmp(lOpcode, opcode[20])) {
				Brn();
			}

			if (!strcmp(lOpcode, opcode[21])) {
				Brzp();
			}

			if (!strcmp(lOpcode, opcode[22])) {
				Brz();
			}

			if (!strcmp(lOpcode, opcode[23])) {
				Brnp();
			}

			if (!strcmp(lOpcode, opcode[24])) {
				Brp();
			}

			if (!strcmp(lOpcode, opcode[25])) {
				Brnz();
			}

			if (!strcmp(lOpcode, opcode[26])) {
				Br();
			}

			if (!strcmp(lOpcode, opcode[27])) {
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

		secondParse();


		printf("successfully did things with files\n");
		fclose(infile);
		fclose(outfile);

	}


}
