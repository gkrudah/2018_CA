#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

typedef struct
{
	char table[6];
	int idx;
}Labeltable;

int read_Label(FILE *, Labeltable*);
int make_machine_code(Labeltable*, char *, char *, char *, char *, int, int);
int offset_err(int);

enum Opcode
{
	add = 0,
	nor,
	lw,
	sw,
	beq,
	jalr,
	halt,
	noop,
	_fill
};

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
		 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	Labeltable table[50];
	int argnum = 0, i = 0, myAddr = 0, machinecode = 0;

	if (argc != 3)
	{
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
	    exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL)
	{
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	
	outFilePtr = fopen(outFileString, "w");

	if (outFilePtr == NULL)
	{
		printf("error in opening %s\n", outFileString);
		exit(1);
	}
	
	argnum = read_Label(inFilePtr, table);

	if(argnum < 1)
	{
		printf("no input\n");
		exit(1);
	}

	rewind(inFilePtr);

	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) )
	{
		machinecode = make_machine_code(table, opcode, arg0, arg1, arg2, argnum, myAddr);

		myAddr++;

		fprintf(outFilePtr, "%d\n", machinecode);
	}
	
	exit(0);//success
	
	return(0);
}

int make_machine_code(Labeltable *table, char *opcode, char *arg0, char *arg1, char *arg2, int argnum, int myAddr)
{
	int i = 0, machinecode = 0, tempop = 0, temp = 0, err = 1;
	enum Opcode _opcode;

	if(!strcmp(opcode, "add"))
		tempop = 0;
	else if(!strcmp(opcode, "nor"))
		tempop = 1;
	else if(!strcmp(opcode, "lw"))
		tempop = 2;
	else if(!strcmp(opcode, "sw")) 
		tempop = 3;
	else if(!strcmp(opcode, "beq"))
		tempop = 4;
	else if(!strcmp(opcode, "jalr"))
		tempop = 5;
	else if(!strcmp(opcode, "halt"))
		tempop = 6;
	else if(!strcmp(opcode, "noop"))
		tempop = 7;
	else if(!strcmp(opcode, ".fill"))
		tempop = 8;
	else
	{
		printf("unrecognized opcode\n");
		exit(1);
	}

	_opcode = tempop;

	switch(_opcode)
	{
		case add:
		case nor:
		case lw:
		case sw:
		case beq:
			machinecode += (_opcode << 22);
			
			temp = atoi(arg0);
			machinecode += (temp << 19);

			temp = atoi(arg1);
			machinecode += (temp << 16);

			if(isNumber(arg2))
			{		
				temp = atoi(arg2);
	
				offset_err(temp);
			}
			else
			{	
				err = 1;

				for(i = 0;i < argnum;i++)
				{
					if(!strcmp(table[i].table, arg2))
					{
						temp = table[i].idx;
						err = 0;
						break;
					}
				}

				if(err == 1)
				{
					printf("undefined label\n");
						exit(1);
				}

				if(_opcode == beq)
				{
					offset_err(temp);
					temp -= (myAddr + 1);
				}
			}
			
			if(_opcode != beq)
				machinecode += temp;
			else
				machinecode += (unsigned short)temp;

			break;
		case jalr:
			machinecode += (_opcode << 22);
			
			temp = atoi(arg0);
			machinecode += (temp << 19);

			temp = atoi(arg1);
			machinecode += (temp << 16);

			break;
		case halt:
		case noop:
			machinecode += (_opcode << 22);

			break;
		case _fill:
			if(isNumber(arg0))
				temp = atoi(arg0);
			else
			{
				err = 1;

				for(i = 0;i < argnum;i++)
				{
					if(!strcmp(table[i].table, arg0))
					{
						temp = table[i].idx;
						err = 0;
						break;
					}
				}

				if(err == 1)
				{
					printf("undefined label\n");
					exit(1);
				}

			}
	
			machinecode += temp;

			break;
	}

	return machinecode;
}

int read_Label(FILE *inFilePtr, Labeltable *table)
{
	char line[MAXLINELENGTH];
	char *ptr = line;
	char temp[MAXLINELENGTH] = {0,};
	int i = 0, j = 0, idx = 0;

	while(fgets(line, MAXLINELENGTH, inFilePtr))
	{
		idx++;
		
		if(strchr(line, '\n') == NULL)
		{
			printf("error: line too long\n");
			exit(1);
		}
		
		ptr = line;
			
		if(isalpha(ptr[0]))
		{
			sscanf(ptr, "%[^\t\n\r ]", temp);

			if(strlen(temp) > 6)	//error handling
			{
				printf("label too long\n");
				exit(1);
			}
			
			for(j = 0;j <= i;j++)
			{
				if(!strcmp(table[j].table, temp))
				{
					printf("duplicate label\n");
					exit(1);
				}
			}

			strcpy(table[i].table, temp);
			table[i++].idx = idx - 1;
		}
	}
		
	return idx;
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
				char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;
	
	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
	
	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
	     return(0);
	if (strchr(line, '\n') == NULL)
	{
		printf("error: line too long\n");
		exit(1);
	}

	ptr = line;

	if (sscanf(ptr, "%[^\t\n\r ]", label))
		ptr += strlen(label);

	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",	opcode, arg0, arg1, arg2);

	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;

	return( (sscanf(string, "%d", &i)) == 1);
}

int offset_err(int offset)
{
	if((offset < -32768) || (32767 < offset))
	{
		printf("offset too big\n");
		exit(1);
	}

	return 0;
}
