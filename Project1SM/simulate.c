#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct
{
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
}stateType;

void printState(stateType *);
int simulator(stateType *);

int main(int argc, char *argv[])
{
	char line[MAXLINELENGTH];
	stateType state;
	FILE *filePtr;
	int i,  exec = 0;

	if (argc != 2)
	{
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}

	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL)
	{
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}

	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
					state.numMemory++)
	{
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1)
		{
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}

	for(i = 0;i < NUMREGS;i++)
		state.reg[i] = 0;
	
	printState(&state);

	for(i = 1;;i++)
	{
		exec = simulator(&state);
		
		if(exec == -1)
		{
			printf("machine halted\n");
			printf("total of %d instructions executed\n", i);
			printState(&state);
			break;
		}
		
		printState(&state);
	}

	return(0);
}

void printState(stateType *statePtr)
{
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	
	for (i=0; i<statePtr->numMemory; i++)
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);

	printf("\tregisters:\n");
	
	for (i=0; i<NUMREGS; i++)
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);									   
    printf("end state\n");
}

//return halt:-1 noop:0 etc:else
int simulator(stateType *statePtr)
{
	int pc, mem, opcode, regA, regB, destreg, offset, temp = 0;
		
	pc = statePtr->pc++;

	mem = statePtr->mem[pc];

	opcode = mem >> 22;
	opcode &= 7;		

	if(opcode == 6) //halt
		return -1;
	else if(opcode == 7) //noop
		return 0;
	
	regA = mem >> 19;
	regA &= 7;

	regB = mem >> 16;
	regB &= 7;

	if(opcode <= 1) //R-type
	{
		destreg = (mem & 7);
		
		if(opcode == 0) //add
			statePtr->reg[destreg] = statePtr->reg[regA] + statePtr->reg[regB];
		else if(opcode == 1) //nor
			statePtr->reg[destreg] = ~(statePtr->reg[regA] | statePtr->reg[regB]);

	}
	else if((2 <= opcode) && (opcode <= 4)) //I-type
	{
		offset = mem;
		offset &= 65535;

		if(offset & (1 << 15))
			offset -= (1 << 16);

		if(opcode == 2) //lw
		{
			temp = statePtr->reg[regA];
			
			if(temp + offset > 65535)
			{
				printf("too long offset\n");
				exit(1);
			}

			statePtr->reg[regB] = statePtr->mem[temp + offset];
		}
		else if(opcode == 3) //sw
		{
			temp = statePtr->reg[regA];
			
			if(temp + offset > 65535)
			{
				printf("too long offset\n");
				exit(1);
			}

			statePtr->mem[temp + offset] = statePtr->reg[regB];
		}
		else if(opcode == 4) //beq
		{
			if(statePtr->reg[regA] == statePtr->reg[regB])		
				statePtr->pc = pc + 1 + offset;
		}
	}
	else if(opcode == 5) //J-type
	{
		statePtr->reg[regB] = pc + 1;
		statePtr->pc = statePtr->reg[regA];
	}

	return 1;
}
