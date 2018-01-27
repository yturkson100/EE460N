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

	if (strlen(str) > 20) {
		return 0;
	}

	if (str[0] == 'x') {
		return 0;
	}

	for (int i = 0; i<3; i++) {
		if (strcmp(str, invalidLabel[i])) {
			return 0;
		}
	}
	for (int i = 0; i < strlen(str); i++) {
		if (!isalpha(str[i]) || !isdigit(str)) {
			return 0;
		}
	}

	return 1;

}

// ------------------------------------------------------


// ********************** First Parse Stuff ************************************

void firstParse() {			// Is that the correct parameter?

	// -------------- Call ReadAndParse() from website ------------
	// Probably needs modifying
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
			/*

			if(isLabel(*lLabel)) {
				createSymbolTable();
			}

			*/
		}
	} while (lRet != DONE);

	//---------------------------------------------------------

	// Get label from file

	// isLabel();


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


		printf("successfully did things with files\n");
		fclose(infile);
		fclose(outfile);

	}


}


// ********************** Second Pass Stuff ************************************

// Translate all opcodes

// File output