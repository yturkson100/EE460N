/*
REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

Name 1: Full name of the first partner
Name 2: Full name of the second partner
UTEID 1: UT EID of the first partner
UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct {

	int PC,		/* program counter */
		N,		/* n condition bit */
		Z,		/* z condition bit */
		P;		/* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
	printf("----------------LC-3b ISIM Help-----------------------\n");
	printf("go               -  run program to completion         \n");
	printf("run n            -  execute program for n instructions\n");
	printf("mdump low high   -  dump memory from low to high      \n");
	printf("rdump            -  dump the register & bus values    \n");
	printf("?                -  display this help menu            \n");
	printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

	process_instruction();
	CURRENT_LATCHES = NEXT_LATCHES;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
	int i;

	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating for %d cycles...\n\n", num_cycles);
	for (i = 0; i < num_cycles; i++) {
		if (CURRENT_LATCHES.PC == 0x0000) {
			RUN_BIT = FALSE;
			printf("Simulator halted\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating...\n\n");
	while (CURRENT_LATCHES.PC != 0x0000)
		cycle();
	RUN_BIT = FALSE;
	printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
	int address; /* this is a byte address */

	printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	printf("-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	printf("\n");

	/* dump the memory contents into the dumpsim file */
	fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	fprintf(dumpsim_file, "-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
	int k;

	printf("\nCurrent register/bus values :\n");
	printf("-------------------------------------\n");
	printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
	printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	printf("\n");

	/* dump the state information into the dumpsim file */
	fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
	fprintf(dumpsim_file, "-------------------------------------\n");
	fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
	fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	fprintf(dumpsim_file, "Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
	char buffer[20];
	int start, stop, cycles;

	printf("LC-3b-SIM> ");

	scanf("%s", buffer);
	printf("\n");

	switch (buffer[0]) {
	case 'G':
	case 'g':
		go();
		break;

	case 'M':
	case 'm':
		scanf("%i %i", &start, &stop);
		mdump(dumpsim_file, start, stop);
		break;

	case '?':
		help();
		break;
	case 'Q':
	case 'q':
		printf("Bye.\n");
		exit(0);

	case 'R':
	case 'r':
		if (buffer[1] == 'd' || buffer[1] == 'D')
			rdump(dumpsim_file);
		else {
			scanf("%d", &cycles);
			run(cycles);
		}
		break;

	default:
		printf("Invalid Command\n");
		break;
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
	int i;

	for (i = 0; i < WORDS_IN_MEM; i++) {
		MEMORY[i][0] = 0;
		MEMORY[i][1] = 0;
	}
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
	FILE * prog;
	int ii, word, program_base;

	/* Open program file. */
	prog = fopen(program_filename, "r");
	if (prog == NULL) {
		printf("Error: Can't open program file %s\n", program_filename);
		exit(-1);
	}

	/* Read in the program. */
	if (fscanf(prog, "%x\n", &word) != EOF)
		program_base = word >> 1;
	else {
		printf("Error: Program file is empty\n");
		exit(-1);
	}

	ii = 0;
	while (fscanf(prog, "%x\n", &word) != EOF) {
		/* Make sure it fits. */
		if (program_base + ii >= WORDS_IN_MEM) {
			printf("Error: Program file %s is too long to fit in memory. %x\n",
				program_filename, ii);
			exit(-1);
		}

		/* Write the word to memory array. */
		MEMORY[program_base + ii][0] = word & 0x00FF;
		MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
		ii++;
	}

	if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

	printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
	int i;

	init_memory();
	for (i = 0; i < num_prog_files; i++) {
		load_program(program_filename);
		while (*program_filename++ != '\0');
	}
	CURRENT_LATCHES.Z = 1;
	NEXT_LATCHES = CURRENT_LATCHES;

	RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
	FILE * dumpsim_file;

	/* Error Checking */
	if (argc < 2) {
		printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
			argv[0]);
		exit(1);
	}

	printf("LC-3b Simulator\n\n");

	initialize(argv[1], argc - 1);

	if ((dumpsim_file = fopen("dumpsim", "w")) == NULL) {
		printf("Error: Can't open dumpsim file\n");
		exit(-1);
	}

	while (1)
		get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
You are allowed to use the following global variables in your
code. These are defined above.

MEMORY

CURRENT_LATCHES
NEXT_LATCHES

You may define your own local/global variables and functions.
You may use the functions to get at the control bits defined
above.

Begin your code here 	  			       */

/***************************************************************/

void setcc(int num) {
	if (num == 0) {
		CURRENT_LATCHES.Z = 1;
	}

	else if (num > 0) {
		CURRENT_LATCHES.P = 1;
	}

	else if (num < 0) {
		CURRENT_LATCHES.N = 1;
	}
}


int sExt(int inSize, int value) {
	/* 
	Dheck if the most sig bit it 1 or 0
	If 1, extend 32bit with 1 
	*/
	int num;

	if (inSize == 5) {
		num = value & 0x10;
		if (num > 0) {
			num = value | 0xfffffff0;
			return num;
		}
		else { return value; }
	}

	if (inSize == 6) {
		num = value & 0x20;
		if (num > 0) {
			num = value | 0xffffffe0;
			return num;
		}
		else { return value; }
	}

	if (inSize == 9) {
		num = value & 0x100;
		if (num > 0) {
			num = value | 0xffffff00;
			return num;
		}
		else { return value; }
	}

	if (inSize == 11) {
		num = value & 0x400;
		if (num > 0) {
			num = value | 0xfffffc00;
			return num;
		}
		else { return value; }
	}

	
}

void process_instruction() {
	/*  function: process_instruction
	*
	*    Process one instruction at a time
	*       -Fetch one instruction
	*       -Decode
	*       -Execute
	*       -Update NEXT_LATCHES
	*/
	int addr = CURRENT_LATCHES.PC;
	int lowBits = MEMORY[addr >> 1][0];
	int highBits = MEMORY[addr >> 1][1];

	int instr = 0x0000;
	int opcode;

	instr = instr | highBits;
	instr = instr << 8;
	instr = instr | lowBits;

	opcode = instr & 0xf000;

	if (opcode == 0x0000) {
		/*	BRANCH	*/
	}

	else if (opcode == 0x1000) {
		/*	ADD	*/
		int sum;
		int dr = instr & 0x0e000;
		dr = dr >> 9;

		int sr1 = instr & 0x01c0;
		sr1 = sr1 >> 6;

		if ((instr & 0x0020) == 0) {
			int sr2 = instr & 0x0007;
			sum = CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2];
			CURRENT_LATCHES.REGS[dr] = sum;
		}
		
		else {
			int imme = instr & 0x001f;
			imme = sExt(5, imme);
			sum = CURRENT_LATCHES.REGS[sr1] + imme;
			sum = Low16bits(sum);
			CURRENT_LATCHES.REGS[dr] = sum;
		}

		setcc(sum);
		CURRENT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}

	else if (opcode == 0x5000) {
		/*	AND	*/
		int andOp;
		int dr = instr & 0x0e000;
		dr = dr >> 9;

		int sr1 = instr & 0x01c0;
		sr1 = sr1 >> 6;

		if ((instr & 0x0020) == 0) {
			int sr2 = instr & 0x0007;
			andOp = CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2];
			CURRENT_LATCHES.REGS[dr] = andOp;
		}

		else {
			int imme = instr & 0x001f;
			imme = sExt(5, imme);
			andOp = CURRENT_LATCHES.REGS[sr1] & imme;
			andOp = Low16bits(andOp);
			CURRENT_LATCHES.REGS[dr] = andOp;
		}
		setcc(andOp);
		CURRENT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

	}

	else if (opcode == 0X4000) {
		/*	JSR	*/
	}

	else if (opcode == 0x2000) {
		/*	LDB	*/
	}

	else if (opcode == 0x6000) {
		/*	LDW	*/
	}

	else if (opcode == 0xE000) {
		/*	LEA	*/
	}

	else if (opcode == 0x9000) {
		/*	NOT	& XOR */
		int dr = instr & 0x0e000;
		dr = dr >> 9;

		int sr = instr & 0x01c0;
		sr = sr >> 6;

		int xorOp;
		int notOp = instr & 0x3f;
		if (notOp == 0x3f) {
			CURRENT_LATCHES.REGS[dr] = ~ CURRENT_LATCHES.REGS[sr];
		}

		else if ((instr & 0x0020) == 0) {
			int sr2 = instr & 0x0007;
			xorOp = CURRENT_LATCHES.REGS[sr] ^ CURRENT_LATCHES.REGS[sr2];
			CURRENT_LATCHES.REGS[dr] = xorOp;
		}
		else if ((instr & 0x0020) > 0) {
			int imme = instr & 0x001f;
			imme = sExt(5, imme);
			xorOp = CURRENT_LATCHES.REGS[sr] ^ imme;
			xorOp = Low16bits(xorOp);
			CURRENT_LATCHES.REGS[dr] = xorOp;
		}

		setcc(xorOp);
		CURRENT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

	}

	else if (opcode == 0xC000) {
		/*	JMP or RET	*/
		int reg = instr & 0x01c0;
		reg = reg >> 6;
		CURRENT_LATCHES.PC = CURRENT_LATCHES.REGS[reg];
	}

	else if (opcode == 0x8000) {
		/*	RTI	*/
	}

	else if (opcode == 0xD000) {
		/*	SHF	*/
	}

	else if (opcode == 0x3000) {
		/*	STB	*/
	}

	else if (opcode == 0x7000) {
		/*	STW	*/
	}

	else if (opcode == 0xF000) {
		/*	TRAP	*/
	}

	NEXT_LATCHES = CURRENT_LATCHES;

}

