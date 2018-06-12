#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for this project */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
	int aluResult;
	int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* number of cycles run so far */
} stateType;

struct hazard_detector {
    int ForwardES;
    int FES_result;
    int ForwardET;
    int FET_result;
    int stall;
};

void printInstruction(int);
int run(stateType *);
void data_hazard_detect(stateType *);
void branch_hazard_detect(stateType *);

struct hazard_detector hazard;

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
	return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
	return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
	return(instruction>>22);
}

void
printInstruction(int instr)
{
	
	char opcodeString[10];
	
	if (opcode(instr) == ADD) {
		strcpy(opcodeString, "add");
	} else if (opcode(instr) == NOR) {
		strcpy(opcodeString, "nor");
	} else if (opcode(instr) == LW) {
		strcpy(opcodeString, "lw");
	} else if (opcode(instr) == SW) {
		strcpy(opcodeString, "sw");
	} else if (opcode(instr) == BEQ) {
		strcpy(opcodeString, "beq");
	} else if (opcode(instr) == JALR) {
		strcpy(opcodeString, "jalr");
	} else if (opcode(instr) == HALT) {
		strcpy(opcodeString, "halt");
	} else if (opcode(instr) == NOOP) {
		strcpy(opcodeString, "noop");
	} else {
		strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr), field2(instr));
}

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
		if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1)
		{
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
	    state.dataMem[state.numMemory] = state.instrMem[state.numMemory];
		printf("memory[%d]=%d\n", state.numMemory, state.dataMem[state.numMemory]);
	}
    
    //initialize
    state.pc = 0;

	for(i = 0;i < NUMREGS;i++)
		state.reg[i] = 0;

    state.IFID.instr = 0x1c00000;
    state.IFID.pcPlus1 = 0;
    state.IDEX.instr = 0x1c00000;
    state.IDEX.pcPlus1 = 0;
    state.IDEX.readRegA = 0;
    state.IDEX.readRegB = 0;
    state.IDEX.offset = 0;
    state.EXMEM.instr = 0x1c00000;
    state.EXMEM.branchTarget = 0;
    state.EXMEM.aluResult = 0;
    state.EXMEM.readRegB = 0;
    state.MEMWB.instr = 0x1c00000;
    state.MEMWB.writeData = 0;
    state.WBEND.instr = 0x1c00000;
    state.WBEND.writeData = 0;
	state.cycles = 0;

    if(run(&state) == -1)
        exit(1);
    
	return(0);
}

int run(stateType *stateptr)
{
    stateType newState;
    stateType state = *stateptr;
    int Opcode = 0, destreg = 0, tmpregA, tmpregB = 0;

    while (1) {
	    printState(&state);

    	/* check for halt */
	    if (opcode(state.MEMWB.instr) == HALT) {
		    printf("machine halted\n");
    		printf("total of %d cycles executed\n", state.cycles);
	    	exit(0);
	    }
		
    	newState = state;
    	newState.cycles++;
				
    	/* --------------------- IF stage --------------------- */
		newState.IFID.instr = state.instrMem[state.pc];
        newState.IFID.pcPlus1 = state.pc + 1;
        newState.pc += 1;
    	/* --------------------- ID stage --------------------- */
		newState.IDEX.instr = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
        newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
        newState.IDEX.offset = field2(state.IFID.instr);
    	/* --------------------- EX stage --------------------- */
		newState.EXMEM.instr = state.IDEX.instr;
        newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
        Opcode = opcode(state.IDEX.instr);
        
        //deal datahazard
        data_hazard_detect(&state);
        
        tmpregA = 0; tmpregB = 0;
                
        if(hazard.ForwardES == 1)
            tmpregA = hazard.FES_result;
        else
            tmpregA = state.IDEX.readRegA;

        if(hazard.ForwardET == 1)
            tmpregB = hazard.FET_result;
        else
            tmpregB = state.IDEX.readRegB;

        if(hazard.stall == 1)
            Opcode = 57411; //stall
        //deal data hazard

        newState.EXMEM.readRegB = tmpregB;

        switch(Opcode)
        {
            case ADD:
                newState.EXMEM.aluResult = tmpregA + tmpregB;
                break;
            case NOR:
                newState.EXMEM.aluResult = ~(tmpregA | tmpregB);
                break;
            case LW:
                newState.EXMEM.aluResult = tmpregA + state.IDEX.offset;
                break;
            case SW:
                newState.EXMEM.aluResult = tmpregA + state.IDEX.offset;
                break;
            case BEQ:
                newState.EXMEM.aluResult = tmpregA - tmpregB;
                break;
            case 57411:
                newState.pc = state.pc;
                newState.IFID.instr = state.IFID.instr;
                newState.IFID.pcPlus1 = state.IFID.pcPlus1;

		        newState.IDEX.instr = state.IDEX.instr; 
                newState.IDEX.pcPlus1 = state.IDEX.pcPlus1;
                newState.IDEX.readRegA = state.IDEX.readRegA;
                newState.IDEX.readRegB = state.IDEX.readRegB;
                newState.IDEX.offset = state.IDEX.offset;

                newState.EXMEM.instr = 0x1c00000;
                newState.EXMEM.branchTarget = 0;
                newState.EXMEM.aluResult = 0;
                newState.EXMEM.readRegB = 0;
                break;
            case HALT:
            case NOOP:
                break;
            default:
                printf("EX stage error\n");
                return -1;
                break;
        }
    	/* --------------------- MEM stage --------------------- */
        newState.MEMWB.instr = state.EXMEM.instr;
		Opcode = opcode(state.EXMEM.instr);

        switch(Opcode)
        {
            case ADD:
            case NOR:
                newState.MEMWB.writeData = state.EXMEM.aluResult;
                break;
            case HALT:
            case NOOP:
                break;
            case LW:
                newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
                break;
            case SW:
                newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
                break;
            case BEQ:
                //deal with branch hazard
                if(state.EXMEM.aluResult == 0)
                {
                    newState.IFID.instr = 0x1c00000;
                    newState.IFID.pcPlus1 = 0;
                    newState.IDEX.instr = 0x1c00000;
                    newState.IDEX.pcPlus1 = 0;
                    newState.IDEX.readRegA = 0;
                    newState.IDEX.readRegB = 0;
                    newState.IDEX.offset = 0;
                    newState.EXMEM.instr = 0x1c00000;
                    newState.EXMEM.branchTarget = 0;
                    newState.EXMEM.aluResult = 0;
                    newState.EXMEM.readRegB = 0;

                    newState.pc = state.EXMEM.branchTarget;
                }
                break;
            default:
                printf("MEM stage error\n");
                return -1;
                break;
        }

    	/* --------------------- WB stage --------------------- */
		newState.WBEND.instr = state.MEMWB.instr;
        Opcode = opcode(state.MEMWB.instr);
        destreg = field2(state.MEMWB.instr) & 0x7;

        switch(Opcode)
        {
            case ADD:
                newState.WBEND.writeData = state.MEMWB.writeData;
                newState.reg[destreg] = state.MEMWB.writeData;
                break;
            case NOR:
                newState.WBEND.writeData = state.MEMWB.writeData;
                newState.reg[destreg] = state.MEMWB.writeData;
                break;
            case LW:
                newState.WBEND.writeData = state.MEMWB.writeData;
                destreg = field1(state.MEMWB.instr);
                newState.reg[destreg] = state.MEMWB.writeData;
                break;
            case SW:
            case BEQ:
            case HALT:
            case NOOP:
                break;
            default:
                printf("WB stage error\n");
                return -1;
                break;
        }

    	state = newState; /* this is the last statement before end of the loop.
					    	 It marks the end of the cycle and updates the
					    	 current state with the values calculated in this
					    	 cycle */
    }

    return 0;
}

//handle struct hazard to deal with data hazard
void data_hazard_detect(stateType *stateptr)
{
    stateType state = *stateptr;

    hazard.ForwardES = 0;
    hazard.FES_result = 0;
    hazard.ForwardET = 0;
    hazard.FET_result = 0;
    hazard.stall = 0;

    int regS = field0(state.IDEX.instr);
    int regT = field1(state.IDEX.instr);    
    int Opcode = opcode(state.MEMWB.instr);
    int reg_hazardE = field2(state.MEMWB.instr) & 0x7;
    
    //instruction before 2 stage
    if(Opcode == ADD || Opcode == NOR)
    {
        if(regS == reg_hazardE)
        {
            hazard.ForwardES = 1;
            hazard.FES_result = state.MEMWB.writeData;
        }

        if(regT == reg_hazardE)
        {
            hazard.ForwardET = 1;
            hazard.FET_result = state.MEMWB.writeData;
        }
    }
    else if(Opcode == LW)
    {
        reg_hazardE = field1(state.MEMWB.instr);

        if(regS == reg_hazardE)
        {
            hazard.ForwardES = 1;
            hazard.FES_result = state.MEMWB.writeData;
        }

        if(regT == reg_hazardE)
        {
            hazard.ForwardET = 1;
            hazard.FET_result = state.MEMWB.writeData;
        }
    }
    
    reg_hazardE = field2(state.EXMEM.instr) & 0x7;
    Opcode = opcode(state.EXMEM.instr);

    //instruction before 1 stage
    //take early reg
    if(Opcode == ADD || Opcode == NOR)
    {
        if(regS == reg_hazardE)
        {
            hazard.ForwardES = 1;
            hazard.FES_result = state.EXMEM.aluResult;
        }

        if(regT == reg_hazardE)
        {
            hazard.ForwardET = 1;
            hazard.FET_result = state.EXMEM.aluResult;
        }
    }
    else if(Opcode == LW)
    {
        if(regS == field1(state.EXMEM.instr) || regT == field1(state.EXMEM.instr))
            hazard.stall = 1;
    }
}
