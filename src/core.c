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
#include <core.h>
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

unsigned long int** stateFileHandler(char* file, unsigned int* ptr){
  FILE *fp;
  
  unsigned int length;
  FILE * f = fopen (file, "r");
  if (f) {
    //Retrieve length
    fseek (f, 0, SEEK_END);
    length = ftell(f);
    rewind(f);
    char buffer[length];
    int count = fread(&buffer, sizeof(char), length, f);
    fclose(f);
    // Printing data to check validity
    //printf("Data read from file: %s \n", buffer);
    //printf("Elements read: %d", count);
  }
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
    return buffer; 
  } else {
    printf("Invalid binary file !");
    *ptr = -1;
    return EXIT_SUCCESS;
  }
}

void fetch(char* code , char* state){
  //Declare size for file contents
  unsigned int code_size = 0;
  unsigned int* cptr_size = &code_size;
  unsigned int state_size = 0;
  unsigned int* sptr_size = &state_size;

  //Retrieve code file content and update size
  uint32_t * int_instr = codeFileHandler(code, cptr_size);
  //uint32_t * reg_val = stateFileHandler(state, sptr_size);
  printf("%x\n",int_instr[1]);
  printf("%x\n", SwapEndian(int_instr[1]));
  //printf("%d\n",code_size);
  printf("Fetch end\n");
}

void decode(){
}

void execute(){
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
        printf("Starting C program with verbose mode...\n");
      } else {
        printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[2],F_OK) == 0) ? "Exists" : "Does't exist", (access(argv[3],F_OK) == 0) ? "Exists" : "Doesn't exist" );
      }
    } else {
     printf("Invalid usage of \"./core\".\nTry \"./core -h\" for more information \n");
    }
  } else if (argc==3){
    // Check if files exist
    if ((access(argv[1],F_OK) == 0) && (access(argv[2],F_OK) == 0)){
      printf("----\nStarting C program...\n");
      fetch(argv[1],argv[2]);
    } else {
      printf("One of the files does not exist.\n code file: %s \n state file: %s \n",(access(argv[1],F_OK) == 0) ? "Exists" : "Does't exist", (access(argv[2],F_OK) == 0) ? "Exists" : "Doesn't exist" );
    }
  } else {
    printf("Invalid usage\nTry \"./core -h\" for more information. \n");
  }

  return EXIT_SUCCESS;
}
