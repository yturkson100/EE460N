#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include <stdbool.h>

const char *opcode[] = { "add","and","halt","jmp","jsr","jsrr","ldb","ldw","lea","nop","not","ret","lshf", "rshfl",
"rshfa","rti","stb","stw","trap","xor","brn","brzp","brz","brnp","brp","brnz","br","brnzp" };
#define MAX_LINE_LENGTH 255
enum { DONE, OK, EMPTY_LINE };
FILE* infile = NULL;
FILE* outfile = NULL;

struct symbol {
	char* label;
	int address;
};

struct table {
	int size;
	int capacity;
	struct symbol** symbolarr;
};


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

struct table* firstParse() {
	int count = 0;
	int startflag = 0;
	int endflag = 0;

	struct table* symboltable = (struct table*)malloc(sizeof(struct table));
	symboltable->size = 0;
	symboltable->capacity = 7;
	symboltable->symbolarr = (struct symbol**)malloc(7 * sizeof(struct symbol));

	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
		*lArg2, *lArg3, *lArg4;

	int lRet;

	//FILE * lInfile;

	//lInfile = fopen( "data.in", "r" );        /* open the input file */

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		if (!strcmp(".end", lOpcode) && startflag == 1) {
			endflag = 1;
		}

		if (lRet != DONE && lRet != EMPTY_LINE && startflag == 1 && endflag == 0) {
			//...
			count++;

			if (strcmp(lLabel, "")) {
				if (isLabel(lLabel) == 0) { exit(1); }
			}

			if (strcmp(lLabel, "") != 0) {
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
				//....
			}
		}

		if (!strcmp(".orig", lOpcode) && endflag == 0) {
			startflag = 1;
		}

	} while (lRet != DONE);
}

int main(int argc, char* argv[]) {
	if (parseArgs(argc, argv)) {
		/* open the source file */
		//fflush(stdout);
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
		struct table* symbolTable = firstParse();
		printf("successfully did things with files\n");
		//---------------------------------------------------------------------------------------
		fclose(infile);
		fclose(outfile);

	}


}