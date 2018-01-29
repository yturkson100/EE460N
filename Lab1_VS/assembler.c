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

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int tableindex = 0;



int parseArgs(int argc, char* argv[]) {
	if (argc == 3) {
		char *prgName = NULL;
		char *iFileName = NULL;
		char *oFileName = NULL;

		prgName = argv[0];
		iFileName = argv[1];
		oFileName = argv[2];

		printf("program name = '%s'\n", prgName);
		printf("input file name = '%s'\n", iFileName);
		printf("output file name = '%s'\n", oFileName);
		fflush(stdout);

		return 1;

	}
	else {
		printf("invalid number of inputs\n");
		fflush(stdout);
		return 0;
	}
}
/*          check functions */
int isOpcode(char* text) {
	for (int i = 0; i<28; i++) {
		if (!strcmp(text, opcode[i])) {
			return 1;
		}
	}

	return -1;
}

int isLabel(char * str) {

	const char *invalidLabel[] = { "in", "out", "getc", "puts", "r0", "r1", "r2", "r3","r4","r5","r6","r7" };

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
	for (int i = 0; i<12; i++) {
		if (!strcmp(str, invalidLabel[i])) {
			return 0;
		}
	}

	// Has to be alpha numeric
	int len = strlen(str);
	for (int i = 0; i < strlen(str); i++) {
		if (!isalpha(str[i]) && !isdigit(str[i])) {
			return 0;
		}
	}

	return 1;

}

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

	while (*lPtr != ';' && *lPtr != '\0' && *lPtr != '\n')
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

int toNum(char * pStr) {
	char * t_ptr;
	char * orig_pStr;
	int t_length, k;
	int lNum, lNeg = 0;
	long int lNumLong;

	orig_pStr = pStr;
	if (*pStr == '#')                                /* decimal */
	{
		pStr++;
		if (*pStr == '-')                                /* dec is negative */
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
		if (*pStr == '-')                                /* hex is negative */
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

int firstParse() {
	int count = 0;
	int endflag = 0;
	int startflag = 0;

	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;

	int lRet;

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		/*checks the very first opcode to be a valid .orig, and set startflag to 1 if it is*/
		if (count == 0 && strcmp(lOpcode, "") != 0) {

			if (strcmp(".orig", lOpcode) != 0) { exit(4); }   //if first opcode isn't .orig, throw error 4

			int value = toNum(lArg1);
			if (value<0 || value>65535) { exit(3); }      //.orig address isn't in 16 bit address space, throw error 3
			if (value % 2 != 0) { exit(3); }                 //.orig address is odd/not word aligned, throw error 3

			startflag = 1;
		}

		/* check for the end of the code and set endflag*/
		if (count != 0 && strcmp(lOpcode, ".end") == 0) {
			endflag = 1;
		}

		if (lRet != DONE && lRet != EMPTY_LINE && startflag == 1 && endflag == 0) {

			count++;

			if (strcmp(lLabel, "") != 0) {               //if it has a label and is valid, check if it already exists, if not put it in the table
				int valid = isLabel(lLabel);
				if (valid == 1) {
					for (int i = 0; i<tableindex; i++) {
						if (strcmp(symbolTable[i].label, lLabel) == 0) {
							exit(4);                                    //error code 4 if label already exists in table
						}
					}
					/* insert label into table*/
					symbolTable[tableindex].address = count - 1;
					strcpy(symbolTable[tableindex].label, lLabel);
					tableindex++;
				}
				else { exit(4); }                                          //error code 4 if the label isn't valid
			}

			//will delete these next 2 lines
			printf("line parsed %i \n", count - 1);
			fflush(stdout);


		}
	} while (lRet != DONE);

	return endflag;
}

int main(int argc, char* argv[]) {
	if (parseArgs(argc, argv)) {
		/* open the source file */
		infile = fopen(argv[1], "r");
		outfile = fopen(argv[2], "w");

		if (!infile) {
			printf("Error: Cannot open file %s\n", argv[1]);
			exit(4);
		}
		if (!outfile) {
			printf("Error: Cannot open file %s\n", argv[2]);
			exit(4);
		}

		/* Do stuff with files */
		//-----get the symbol table----------
		int end = firstParse();
		if (end != 1) { exit(4); }                    //throw an error if there is no .end in code
		printf("successfully did things with files\n");
		fflush(stdout);
		//---------------------------------------------------------------------------------------
		fclose(infile);
		fclose(outfile);

	}

}