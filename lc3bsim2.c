/*
    Remove all unnecessary lines (including this one) 
    in this comment.
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


typedef struct System_Latches_Struct{

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

  switch(buffer[0]) {
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

  for (i=0; i < WORDS_IN_MEM; i++) {
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
void initialize(char *files[], int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(files[i]);
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

  initialize(&argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
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

void set_condition_codes(int DR){
  if (NEXT_LATCHES.REGS[DR] == 0) {
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 1;
    NEXT_LATCHES.P = 0;
  } else if (NEXT_LATCHES.REGS[DR] & 0x8000) {  // Negative
    NEXT_LATCHES.N = 1;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
  } else {  // Positive
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 1;
  }
}

int sext(int num, int msb) {
  int mask = 1 << (msb - 1);
  return (num ^ mask) - mask;
}

void add_instruction(int instruction){
  int DR = (instruction >> 9) & 0x7;
  int SR1 = (instruction >> 6) & 0x7;
  
  if ((instruction >> 5) & 0x1) {  // For immediate mode
    int imm5 = instruction & 0x1F;
    imm5 = sext(imm5, 4);  // Sext it
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + imm5);
  } else {  // For Register mode
    int SR2 = instruction & 0x7;  // Source register 2
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
  }
  set_condition_codes(DR);
}

void and_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int SR1 = (instruction >> 6) & 0x7;
  
  if ((instruction >> 5) & 0x1) {  // For immediate mode
    int imm5 = instruction & 0x1F;
    imm5 = sext(imm5, 4);  // Sext it
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & imm5);
  } else {  // For Register mode
    int SR2 = instruction & 0x7;  // Source register 2
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2]);
  }
  set_condition_codes(DR);
}

void br_instruction(int instruction) {
  int n = (instruction >> 11) & 0x1;
  int z = (instruction >> 10) & 0x1;
  int p = (instruction >> 9) & 0x1;

  if((n && CURRENT_LATCHES.N) || (p && CURRENT_LATCHES.P) || (z && CURRENT_LATCHES.P)){
    int offset9 = instruction & 0x01FF;
    NEXT_LATCHES.PC += (sext(offset9, 8) << 1);
  }
}

void jmp_instruction(int instruction) {
  int BaseR = (instruction >> 6) & 0x7;
  if(BaseR == 15){ // JMP
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[7];
  } else {
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR];
  }
}

void jsr_instruction(int instruction) {
  NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[7];
  int steer_bit = (instruction >> 11) & 0x1;
  if (steer_bit) {
    int offset11 = instruction & 0x07FF;
    NEXT_LATCHES.PC = (sext(instruction, 10) << 1);
  } else {
    int BaseR = (instruction >> 6) & 0x7;
    NEXT_LATCHES.PC += CURRENT_LATCHES.REGS[BaseR];
  }
}

void ldb_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int BaseR = (instruction >> 6) & 0x7;
  int boffset6 = instruction & 0x3F;

  NEXT_LATCHES.REGS[DR] = Low16bits(sext((MEMORY[CURRENT_LATCHES.REGS[BaseR] + sext(boffset6, 5)][0]) & 0x0FF, 7));
  set_condition_codes(DR);
}

void ldw_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int BaseR = (instruction >> 6) & 0x7;
  int boffset6 = instruction & 0x3F;

  NEXT_LATCHES.REGS[DR] = Low16bits(MEMORY[CURRENT_LATCHES.REGS[BaseR] + (sext(boffset6, 5) << 1)][0]);
  NEXT_LATCHES.REGS[DR] |= MEMORY[CURRENT_LATCHES.REGS[BaseR] + (sext(boffset6, 5) << 1)][1];
  set_condition_codes(DR);
}

void lea_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int offset9 = instruction & 0x01FF;
  NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + (sext(offset9, 8) << 1));
}

void shf_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int SR = (instruction >> 6) & 0x7;
  int amount4 = instruction & 0xF;
  int steer_bits = (instruction >> 4) & 0x3;

  if (steer_bits == 0) { // LSHF
    CURRENT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] << amount4);
  } else if (steer_bits == 1) { // RSHFL
    CURRENT_LATCHES.REGS[DR] = Low16bits((int) ((unsigned int) CURRENT_LATCHES.REGS[SR] >> amount4));
  } else if (steer_bits == 3) { // RSHFA
    CURRENT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] >> amount4);
  }
}

void stb_instruction(int instruction) {
  int SR = (instruction >> 9) & 0x7;
  int BaseR = (instruction >> 6) & 0x7;
  int offset6 = instruction & 0x3F;
  int address = (CURRENT_LATCHES.REGS[BaseR] + sext(offset6, 5));

  MEMORY[address >> 1][address & 0x1] = CURRENT_LATCHES.REGS[SR] >> 8;
}

void stw_instruction(int instruction) {
  int SR = (instruction >> 9) & 0x7;
  int BaseR = (instruction >> 6) & 0x7;
  int offset6 = instruction & 0x3F;
  int address = CURRENT_LATCHES.REGS[BaseR] + (sext(offset6, 5) << 1);

  MEMORY[address >> 1][0] = CURRENT_LATCHES.REGS[SR] & 0xFF;
  MEMORY[address >> 1][1] = (CURRENT_LATCHES.REGS[SR] >> 8) & 0xFF;
}

void trap_instruction(int instruction) {
    int trapvect8 = instruction & 0xFF;

    CURRENT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;

    CURRENT_LATCHES.PC = Low16bits(MEMORY[trapvect8][1] << 8 | MEMORY[trapvect8][0]);
}

void xor_instruction(int instruction) {
  int DR = (instruction >> 9) & 0x7;
  int SR1 = (instruction >> 6) & 0x7;

  if ((instruction >> 5) & 0x1) { // Immediate
    int imm5 = instruction & 0x1F;
    imm5 = sext(imm5, 4);
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ imm5);
  } else {  // Register
    int SR2 = instruction & 0x7;
    NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]);
  }

  set_condition_codes(DR);
}

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     

  int instruction = (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8) | MEMORY[CURRENT_LATCHES.PC >> 1][0];
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

  int opcode = (instruction >> 12) & 0xF;

  switch (opcode) {
        case 0x1:  // ADD
            add_instruction(instruction);
            break;
        case 0x5:  // AND
            and_instruction(instruction);
            break;
        case 0x0:  // BR
            br_instruction(instruction);
            break;
        case 0xC:  // JMP/RET
            jmp_instruction(instruction);
            break;
        case 0x4:  // JSR/JSRR
            jsr_instruction(instruction);
            break;
        case 0x2:  // LDB
            ldb_instruction(instruction);
            break;
        case 0x6:  // LDW
            ldw_instruction(instruction);
            break;
        case 0xE:  // LEA
            lea_instruction(instruction);
            break;
        case 0xD:  // SHF
            shf_instruction(instruction);
            break;
        case 0x3:  // STB
            stb_instruction(instruction);
            break;
        case 0x7:  // STW
            stw_instruction(instruction);
            break;
        case 0xF:  // TRAP
            trap_instruction(instruction);
            break;
        case 0x9:  // XOR
            xor_instruction(instruction);
            break;
        default:
            printf("Unknown opcode: %x\n", opcode);
            break;
    }
}
