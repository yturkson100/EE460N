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

int offset9conversion(int input) {
	int negflag = 0;
	int mask = 0x000;
	if (input<0) {
		negflag = 1;
		input = -input;
	}

	int value = mask | input;
	if (negflag == 1) {
		value = ~value;
		value = value + 1;
	}

	value = value & 0x01ff;
	// printf("%04x",value);
	// fflush(stdout);
	return value;
}

/*------------------functions-----------------------------------------------*/
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

		// rangeCheck()

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

		// rangeCheck()

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

void Ldb() {

}

void Ldw() {

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

	// rangeCheck()


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

	// rangeCheck()

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

	// rangeCheck()

	num = toNum(lArg3);
	num = num & 0x000f;
	value = value | num;
	return value;

}

void Stb() {

}

void Stw() {

}

void Trap() {

}

int Xor(char *lArg1, char *lArg2, char *lArg3, char *lArg4) {
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

		// rangeCheck()

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

int Branch(char *lArg1, char *lArg2, char *lArg3, char *lArg4, int count, char* lOpcode) {
	int ans = 0x0000;
	int labelcheck = 0;
	int labelindex;
	int destination;
	int offset;
	if ((strcmp(lArg1, "") == 0) || (strcmp(lArg2, "") != 0)) { exit(4); }     //invalid number of operands 0 or more than 1

	for (int i = 0; i<tableindex; i++) {
		if (strcmp(lArg1, symbolTable[i].label) == 0) {                  //if label exists, mark the index
			labelcheck = 1;
			labelindex = i;
		}
	}

	if (labelcheck != 1) { exit(1); }                                     //if label is undefined, exit with error code 1

	destination = symbolTable[labelindex].address;
	offset = destination - count;
	if (offset>255 || offset<-256) { exit(4); }                         // if offset cannot be represented, exit with error code 4
	int value = offset9conversion(offset);

	ans = ans | value;                                                //add the offset to instruction

	if (strcmp(lOpcode, "br") == 0 || strcmp(lOpcode, "brnzp") == 0) {         // BRNZP and br
		ans = ans | 0x0e00;
	}
	else if (strcmp(lOpcode, "brn") == 0) {
		ans = ans | 0x0800;
	}
	else if (strcmp(lOpcode, "brz") == 0) {
		ans = ans | 0x0400;
	}
	else if (strcmp(lOpcode, "brp") == 0) {
		ans = ans | 0x0200;
	}
	else if (strcmp(lOpcode, "brzp") == 0) {
		ans = ans | 0x0600;
	}
	else if (strcmp(lOpcode, "brnp") == 0) {
		ans = ans | 0x0a00;
	}
	else if (strcmp(lOpcode, "brnz") == 0) {
		ans = ans | 0x0c00;
	}
	return ans;
}


//-----------------------end of functions-------------------------------------------------------

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

void secondParse() {
	int count = 0;
	int endflag = 0;
	int startflag = 0;

	// Call ReadAndParse()
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;

	int lRet;

	do
	{
		lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);

		/*set the start flag when opcode is .orig*/                                                 //made change
		if (count == 0 && strcmp(lOpcode, ".orig") == 0) {
			startflag = 1;
		}

		/* check for the end of the code and set endflag*/                                          //made change
		if (count != 0 && strcmp(lOpcode, ".end") == 0) {
			endflag = 1;
		}

		if (lRet != DONE && lRet != EMPTY_LINE && startflag == 1 && endflag == 0) {
			count++;

			// -------------- Identify opcodes ------------------------

			//The trap vector for a TRAP instruction and the shift amount for SHF instructions must be non-negative values.

			if (!strcmp(lOpcode, opcode[0])) {

				/*	ADD()	 */

				int num = Add(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}
			//.Orig
			else if (!strcmp(lOpcode, ".orig")) {
				//remember to print to file
				if (strcmp(lArg2, "") != 0) { exit(4); }           //error code 4 if .orig has more than one operand
				int ans = 0x0000;
				int value = toNum(lArg1);
				ans = ans | value;
				printf("0x%.4X\n", ans);
				fflush(stdout);
			}

			//.fill
			//            else if(!strcmp(lOpcode,".fill")){
			//                //remember to print to file
			//                if(strcmp(lArg1,"")==0 || strcmp(lArg2,"")!=0){exit(4);}   //invalid number of arguments
			//                int ans = 0x0000;
			//                int value = toNum(lArg1);
			//                if(value<0 || value>65535){exit()}                          //value cannot be represented with 16 bits
			//
			//
			//            }

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
				Ldb();
			}

			else if (!strcmp(lOpcode, opcode[7])) {
				Ldw();
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

				int num = Rshfl(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);

			}

			else if (!strcmp(lOpcode, opcode[14])) {

				/*	Rsha()  */

				int num = Rshfa(lArg1, lArg2, lArg3, lArg4);
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
				Stb();
			}

			else if (!strcmp(lOpcode, opcode[17])) {
				Stw();
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

				int num = Xor(lArg1, lArg2, lArg3, lArg4);
				printf("0x%.4X\n", num);

				// File Output:
				//fprintf(outfile, "0x%.4X\n", num);
			}

			//-----------------------branches-------------------------------------------------------
			else if (memcmp(lOpcode, "br", 2) == 0) {
				if (isOpcode(lOpcode)) {                          // if it's one of the valid branches
					int value = Branch(lArg1, lArg2, lArg3, lArg4, count, lOpcode);
					printf("0x%04x\n", value);
					fflush(stdout);


				}
			}

			else if (!strcmp(lOpcode, ".fill")) {
				printf("line filler\n");
				fflush(stdout);
			}

			// File output

		}
	} while (lRet != DONE);

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
		printf("successfully completed first parse\n");
		fflush(stdout);
		//---------------------------------------------------------------------------------------
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		rewind(infile);

		//----second parse and decode instructions------------------------------

		secondParse();
		printf("successfully completed second parse\n");
		fflush(stdout);


		fclose(infile);
		fclose(outfile);

	}

}