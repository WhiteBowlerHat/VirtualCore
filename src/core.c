/* 

   Example of Virtual Core 
   Authors:  Bilal Taalbi & Maxime Rouhier
   Last update : 09/12/2021
 
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <core.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include <inttypes.h>
#include <limits.h>


//Convert little endian to big endian (and reverse)
uint32_t SwapEndian(uint32_t num){
  uint32_t b0,b1,b2,b3;
  uint32_t res;

  b0 = (num & 0x000000ff) << 24u;
  b1 = (num & 0x0000ff00) << 8u;
  b2 = (num & 0x00ff0000) >> 8u;
  b3 = (num & 0xff000000) >> 24u;

  return res = b0 | b1 | b2 | b3;
}

//Check if a char is in array
int isInArray(char *arr[],char *x, int arrLen){
  int isElementPresent = 0;
     
  for (int i = 0; i < arrLen; i++) {
      if (strcmp(arr[i],x)==0) {
          isElementPresent = 1;
          break;
      }
  }
  return isElementPresent;
}

// pow() function for long type
long ipow(long base, long exp)
{
    long result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }
    return result;
}

//Convert Hex char to Integer
int digit_to_int(char d)
{
 char str[2];
 str[0] = d;
 str[1] = '\0';
 return (int) strtol(str, NULL, 10);
}


//Count the number of lines in a file
// TO DO: fix empty lines
uint32_t countlines(FILE *file) {
  uint32_t lines = 0;
  int32_t c;
  int32_t last = '\n';
  while (EOF != (c = fgetc(file))) {
    if (c == '\n' && last != '\n') {
      ++lines;
    }
    last = c;
  }
  /* Reset the file pointer to the start of the file */
  rewind(file);
  return lines;
}


uint64_t* stateFileHandler(char* file, unsigned int* ptr){
  //Variables intitalization
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int length=0;
  int j=0;
  char *subString;
  uint64_t* res;

  //Opening file
  fp = fopen(file, "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  
  //retrieve length -> create table of registeries and PC with the length and 
  length = countlines(fp);
  res = (uint64_t*) malloc (length*sizeof(uint64_t));
  *ptr=length;
 
  //read the file and update registeries value
  while ((read = getline(&line, &len, fp)) != -1) { 
      char linex[len];
      strcpy(linex, line);
      subString = strtok(linex,"x"); // find the x
      subString = strtok(NULL,"\n");   // find the \n
      res[j] = (uint64_t)strtoull(subString, NULL, 16);
      j=j+1;
    
  }
  printf("NUMBER OF REGISTERIES LOADED  : %d\n", *ptr);
  printf("INITIAL STATE [" );
    for (int i =0 ; i<*ptr; i++){
      uint64_t p = res[i];
      printf("r%d=%016lx\n", i, (uint64_t) p);
    }
    printf("]\n---\n");
  return res;
  
  fclose(fp);
  if (line)
      free(line);
  exit(EXIT_SUCCESS);
}

//Reads the binary file where all the instructions are stored and put them in a 32 bits array
uint32_t * codeFileHandler(char* file, unsigned int* ptr){
  FILE *fp;
  //Declare type as 32 bits Integer
  uint32_t * buffer;
  unsigned int length;
  //Open file as binary
  FILE * f = fopen (file, "rb");
  if (f) { 
    //Retrieve length
    fseek (f, 0, SEEK_END);
    length = ftell(f);
    rewind(f);
    //Get file content
    buffer = (uint32_t*) malloc (length);
    if (buffer) {
      fread (buffer, length, 1, f);
    }
    //Update size
    *ptr = length/4;
    fclose (f);
    for (int i = 0; i < length/4; i++) {
      buffer[i] = SwapEndian(buffer[i]);
    }
    return buffer; 
  } else {
    printf("Invalid binary file !");
    *ptr = -1;
    return EXIT_SUCCESS;
  }
}

// This function read the instructions and update the program counter according to different
uint32_t fetch(uint32_t instr, int verbose, long* pc, long* c1, long* c2){

  uint8_t bcc = (instr & 0xf0000000)/ 0x10000000;
  long off = (instr & 0x07ffffff);
  uint8_t flag = (instr & 0x0f000000)/ 0x1000000;

  int n = flag>>3;
  if (bcc == 8){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 9) && (*c1 == *c2)){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 10) && (*c1 != *c2)){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 11) && (*c1 <= *c2)){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 12) &&  (*c1 >= *c2)){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 13) && (*c1 < *c2)){
    *pc = *pc+ipow((-1),n)*off;
  } else if ((bcc == 14) && (*c1 > *c2)) {
    *pc = *pc+ipow((-1),n)*off;
  } else if (bcc == 8) {
    *pc = *pc+1;
  } else {
    *pc = *pc+1;
  }
  if (verbose == 1){
    printf("BCC : %x; PC: %ld ; Offset : %lx", bcc,*pc,  off);
  }
  return instr;
}

//This function decode an instructions and puts everything into an array
void decode(uint32_t instr, int verbose, uint32_t* arr){
  uint32_t bcc = (instr & 0xf0000000)/ 0x10000000;
  uint32_t off = (instr & 0x07ffffff);
  uint32_t ivf = (instr & 0x01000000)/0x1000000 ;
  uint32_t opcode = (instr & 0x00f00000) /0x100000;
  uint32_t first_op = (instr & 0x000f0000) /0x10000;
  uint32_t second_op = (instr & 0x0000f000) /0x1000;
  uint32_t dest_reg = (instr & 0x00000f00) /0x100;
  uint32_t iv = (instr & 0x000000ff);
    arr[0] = ivf;
    arr[1] = opcode;
    arr[2] = first_op;
    arr[3] = second_op;
    arr[4] = dest_reg;
    arr[5] = iv;
    arr[6] = bcc;
    if (verbose == 1)
    if (bcc != 0){
      printf("BCC : %x; Offset %x", bcc, off);
    } else {
      printf("ivf : %x; opcode : %x; first_op : %x; second_op : %x; dest_reg : %x; iv : %x", ivf,opcode, first_op, second_op, dest_reg, iv);
    } 
}

//This function execute the instruction and update the registeries value according to multiple operations 
//See the array above to see which index has which value
void execute(uint32_t* decoded_instr, uint64_t* registeries, long* carry, long* c1, long* c2, unsigned int * sptr_size, int verbose){
  uint64_t temp;
  
  //Update the temp value according to the immediate value flag 
  if (decoded_instr[0] == 0 ){// ivf =  decode_instr[0] (see above)
    temp = registeries[decoded_instr[3]];
  } else {
    temp = decoded_instr[5];
  }

  if (decoded_instr[6] == 0) {
    switch(decoded_instr[1])
    {
      case 0: //AND
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] & temp;
          break;
      case 1: //OR
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] | temp;
          break;
      case 2: //XOR
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] ^ temp;
          break;
      case 3: //ADD
          if ((temp > 0) && (registeries[decoded_instr[2]] > 0xfffffffffffffff - temp))// if overflow
            *carry = 1;
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] + temp;
          break;
      case 4: //ADC
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] + temp + *carry;
          *carry = 0;
          break;
      case 5: //CMP
          *c1 = registeries[decoded_instr[2]];
          *c2 = temp;
          break;
      case 6: //SUB
          if ((temp < 0) && (registeries[decoded_instr[2]] > 0xfffffffffffffff + temp))
            *carry = 1;
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] - temp;
          break;
      case 7: //SBC
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] - temp + *carry -1;
          break;
      case 8: //MOV
          registeries[decoded_instr[4]] = temp;
          break;
      case 9: //LSH
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] << temp;
          break;
      case 10: //RSH
          registeries[decoded_instr[4]] = registeries[decoded_instr[2]] >> temp;
          break;
      default :
          registeries[decoded_instr[4]] = registeries[decoded_instr[4]];
          break;
    }
  }
  
  if (verbose ==1) {
    printf("\n   -Carry = %ld \n   -Registeries : [",*carry);
    for (int i =0 ; i<*sptr_size; i++){
      uint64_t p = registeries[i];
      printf("r%d=%016lx;", i,(uint64_t) p);
    }
    printf("]\n   -C1 = %ld ; C2 = %ld\n", *c1, *c2);
  }
}

// Main function of the core 
// It initialize necessary variables and go through fetch, decode and execute
void core(char* code , char* state, int verbose){
  //Declare PC
  long pc = 0;
  long * ptr_pc = &pc;
  //Declare compare memory values
  long c1 = 0;
  long * ptr_c1 = &c1;
  long c2 = 0;
  long * ptr_c2 = &c2;
  //Declare size for file contents
  unsigned int code_size = 0;
  unsigned int* cptr_size = &code_size;
  unsigned int state_size = 0;
  unsigned int* sptr_size = &state_size;
  //Declare carry
  long carry = 0;
  long* ptr_carry = &carry;
  uint32_t current_instr;
  
  //Retrieve code file content and update size
  uint32_t * int_instr = codeFileHandler(code, cptr_size);
  
  //Retrieve state file content and initialize registers
  uint64_t* registeries = stateFileHandler(state, sptr_size);
  
  uint32_t* decoded_instr;
  decoded_instr = (uint32_t*) calloc(7,sizeof(uint32_t)); 
  while (*ptr_pc != *cptr_size ){
    if (verbose == 1)
      printf("Fetch :");
    current_instr = fetch(int_instr[*ptr_pc],verbose,ptr_pc,ptr_c1,ptr_c2);
    if (verbose == 1)
      printf("\n");
    if (verbose == 1)
      printf("Decode :");
    decode(current_instr,verbose, decoded_instr);
    if (verbose == 1)
      printf("\n");
    if (verbose == 1)
      printf("Execute :");
    execute(decoded_instr,registeries,ptr_carry, ptr_c1,ptr_c2,sptr_size, verbose);
    if (verbose == 1)
      printf("\n---\n");
  }

  printf("Final state of registeries :\n");
  for (int i =0 ; i<*sptr_size; i++){
    uint64_t p = registeries[i];
    printf("r%d=%016lx\n", i, (uint64_t) p);
  }
    printf("\n");
  free(registeries);
  free(int_instr);
}


//Handle the user interactions when they start the program
//Provide help if needed "-h"
int main(int argc, char *argv[])
{
  if (argc==1){
    printf("Invalid usage of \"./core\".\nTry \"./core -h\" for more information \n\nIn order to see the help command your computer needs to be able to run \"man\"\n");
  } else if (isInArray(argv,"-h", argc)){
    system("man ./coreman");
  } else if (argc==4){
    if (strcmp(argv[1],"-v")==0) {
      // Check if files exist
      if ((access(argv[2],F_OK) == 0) && (access(argv[3],F_OK) == 0)){
        printf("-------------------------\nVIRTUAL CORE RUNNING...\n-------------------------\n");
        printf("     _----------_,\n    ,\"__         _-:, \n   /    \"\"--_--\"\"...:\\\n  /         |.........\\\n /          |..........\\\n/,         _'_........./:\n! -,    _-\"   \"-_... ,;;:\n\\   -_-\"         \"-_/;;;;\n \\   \\             /;;;;'\n  \\   \\           /;;;;\n   '.  \\         /;;;'\n     \"-_\\_______/;;'\n");
        printf("-------------------------\nVERBOSE : ENABLED\n");
        core(argv[2],argv[3],1);
      } else {
        printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[2],F_OK) == 0) ? "Exists" : "Doesn't exist", (access(argv[3],F_OK) == 0) ? "Exists" : "Doesn't exist" );
      }
    } else {
     printf("Invalid usage of \"./core\".\nTry \"./core -h\" for more information \n\nIn order to see the help command your computer needs to be able to run \"man\"\n");
    }
  } else if (argc==3){
    // Check if files exist
    if ((access(argv[1],F_OK) == 0) && (access(argv[2],F_OK) == 0)){
      printf("-------------------------\nVIRTUAL CORE RUNNING...\n-------------------------\n");
      printf("     _----------_,\n    ,\"__         _-:, \n   /    \"\"--_--\"\"...:\\\n  /         |.........\\\n /          |..........\\\n/,         _'_........./:\n! -,    _-\"   \"-_... ,;;:\n\\   -_-\"         \"-_/;;;;\n \\   \\             /;;;;'\n  \\   \\           /;;;;\n   '.  \\         /;;;'\n     \"-_\\_______/;;'\n");
      printf("-------------------------\nVERBOSE : DISABLED\n");
      core(argv[1],argv[2],0);
    } else {
      printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[1],F_OK) == 0) ? "Exists" : "Doesn't exist", (access(argv[2],F_OK) == 0) ? "Exists" : "Doesn't exist" );
    }
  } else {
    printf("Invalid usage\nTry \"./core -h\" for more information. \n\nIn order to see the help command your computer needs to be able to run \"man\"\n");
  }

  return EXIT_SUCCESS;
}
