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
#include <math.h>
#include <inttypes.h>

uint32_t SwapEndian(uint32_t num){
  uint32_t b0,b1,b2,b3;
  uint32_t res;

  b0 = (num & 0x000000ff) << 24u;
  b1 = (num & 0x0000ff00) << 8u;
  b2 = (num & 0x00ff0000) >> 8u;
  b3 = (num & 0xff000000) >> 24u;

  return res = b0 | b1 | b2 | b3;
}

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


int digit_to_int(char d)
{
 char str[2];
 str[0] = d;
 str[1] = '\0';
 return (int) strtol(str, NULL, 10);
}


long* stateFileHandler(char* file, unsigned int* ptr){
  FILE * fp;
  char * line = NULL;
  
  size_t len = 0;
  ssize_t read;
  int length=0;
  int j=0;
  char *subString;
  long* res;

  fp = fopen(file, "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  //Retrieve length
  fseek (fp, 0, SEEK_END);
  length = ftell(fp);
  rewind(fp);
  //Update size
  *ptr = length;
  res = (long*) malloc (length);
 
  while ((read = getline(&line, &len, fp)) != -1) { 
    if (line[0]=='r'){
      if (line[2]=='='){
        int k=digit_to_int(line[1]);
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s",line);
        if (k>-1 || k<10){
          char linex[len];
          strcpy(linex, line);
          subString = strtok(linex,"x"); // find the x
          subString = strtok(NULL,"\n");   // find the \n
          res[j] = (int)strtol(subString, NULL, 16);
        // printf("v=%lx\n", res[j]);
          j=j+1;
        }
      } else if (line[3]=='='){
        int k=digit_to_int(line[1]);
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s",line);
        if (k>-1 || k<10){
          char linex[len];
          strcpy(linex, line);
          subString = strtok(linex,"x"); // find the x
          subString = strtok(NULL,"\n");   // find the \n
          res[j] = (int)strtol(subString, NULL, 16);
         // printf("v=%lx\n", res[j]);
          j=j+1;
        }
      }
    } else {
      //printf("Retrieved line of length %zu:\n", read);
      //printf("%d\n", digit_to_int(line[1])*10+digit_to_int(line[2]));
    }
  }
  
 
  return res;
  fclose(fp);
  if (line)
      free(line);
  exit(EXIT_SUCCESS);
}


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

uint32_t fetch(uint32_t instr, int verbose, long* pc, long* c1, long* c2){
  //printf("%x\n",instr);
  uint8_t bcc = (instr & 0xf0000000)/ 0x10000000;
  long off = (instr & 0x07ffffff);
  //off = off >> 21;
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

void decode(uint32_t instr, int verbose, uint32_t* arr){
  uint32_t bcc = (instr & 0xf0000000)/ 0x10000000;
  uint32_t off = (instr & 0x07ffffff);
  uint32_t ivf = (instr & 0x01000000) /1000000;
  uint32_t opcode = (instr & 0x00f00000) /100000;
  uint32_t first_op = (instr & 0x000f0000) /10000;
  uint32_t second_op = (instr & 0x0000f000) /1000;
  uint32_t dest_reg = (instr & 0x00000f00) /100;
  uint32_t iv = (instr & 0x000000ff);
  /*if (bcc != 0) {
    arr = (uint32_t*) calloc(6,sizeof(uint32_t));
    printf("ivf : %x; opcode : %x; first_op : %x; second_op : %x; dest_reg : %x; iv : %x", ivf,opcode, first_op, second_op, dest_reg, iv);
    if (verbose == 1)
    printf("BCC : %x; Offset %x", bcc, off);
  } else {*/
    arr = (uint32_t*) calloc(7,sizeof(uint32_t)); 
    arr[0] = ivf;
    arr[1] = opcode;
    arr[2] = first_op;
    arr[4] = second_op;
    arr[5] = dest_reg;
    arr[6] = iv;
    arr[7] = bcc;
    if (verbose == 1)
    if (bcc != 0){
      printf("BCC : %x; Offset %x", bcc, off);
    } else {
      printf("ivf : %x; opcode : %x; first_op : %x; second_op : %x; dest_reg : %x; iv : %x", ivf,opcode, first_op, second_op, dest_reg, iv);
    }
    
    
  //}
  
}

void execute(uint32_t* decoded_instr, long* registeries){

}


void core(char* code , char* state, int verbose){
  //Declare PC
  long pc = 0;
  long * ptr_pc = &pc;
  //Declare compare memory values
  long c1 = 0;
  long * ptr_c1 = &c1;
  long c2 = 1;
  long * ptr_c2 = &c2;
  //Declare size for file contents
  unsigned int code_size = 0;
  unsigned int* cptr_size = &code_size;
  unsigned int state_size = 0;
  unsigned int* sptr_size = &state_size;
  //
  uint32_t current_instr;
  
  //Retrieve code file content and update size
  uint32_t * int_instr = codeFileHandler(code, cptr_size);
  //printf("%x\n",int_instr[1]);
  
  //Retrieve state file content and initialize registers
  long* registeries = stateFileHandler(state, sptr_size);
  //printf("%lx\n",registeries[15]);
  while (*ptr_pc != *cptr_size ){
    uint32_t* decoded_instr;
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
    execute(decoded_instr,registeries);
    if (verbose == 1)
      printf("\n---\n");
  }
  free(registeries);
  free(int_instr);
}



int main(int argc, char *argv[])
{
  //Handle the user interactions when they start the program
  if (argc==1){
    printf("Invalid usage of \"./core\".\nTry \"./core -h\" for more information \n");
  } else if (isInArray(argv,"-h", argc)){
    system("man ./coreman");
  } else if (argc==4){
    if (strcmp(argv[1],"-v")==0) {
      // Check if files exist
      if ((access(argv[2],F_OK) == 0) && (access(argv[3],F_OK) == 0)){
        printf("----\nStarting C program with verbose mode...\n---\n");
        core(argv[2],argv[3],1);
      } else {
        printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[2],F_OK) == 0) ? "Exists" : "Doesn't exist", (access(argv[3],F_OK) == 0) ? "Exists" : "Doesn't exist" );
      }
    } else {
     printf("Invalid usage of \"./core\".\nTry \"./core -h\" for more information \n");
    }
  } else if (argc==3){
    // Check if files exist
    if ((access(argv[1],F_OK) == 0) && (access(argv[2],F_OK) == 0)){
      printf("----\nStarting C program...\n");
      core(argv[1],argv[2],0);
    } else {
      printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[1],F_OK) == 0) ? "Exists" : "Doesn't exist", (access(argv[2],F_OK) == 0) ? "Exists" : "Doesn't exist" );
    }
  } else {
    printf("Invalid usage\nTry \"./core -h\" for more information. \n");
  }

  return EXIT_SUCCESS;
}
