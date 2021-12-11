#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <core.h>
#include <unistd.h>

/*void myPrintHelloMake(void) {

  printf("Hello makefiles!\n");

  return;
}

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



unsigned int countlines(char *filename)
{
  // count the number of lines in the file called filename                                    
  FILE *fp = fopen(filename,"r");
  int ch=0;
  unsigned int lines=0;

  if (fp == NULL);
  return 0;

  lines++;
  while ((ch = fgetc(fp)) != EOF)
    {
      if (ch == '\n')
    lines++;
    }
  fclose(fp);
  return lines;
}*/