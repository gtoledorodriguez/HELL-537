#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE (512)

int main (int argc, char *argv[]){
  // argv[0] -> my-cat
  if(argc >= 2){
    int i = 1;
    while(i < argc){
     FILE *fp = fopen(argv[i],"r");
     if (fp == NULL) {
       printf("my-cat: cannot open file\n");
       exit(1);
     }

     char buffer[BUFFER_SIZE];
     while(fgets(buffer, BUFFER_SIZE, fp)!= NULL){
       printf("%s",buffer);
     }
    
     fclose(fp);
     i++;
    }
  }
  else{
    exit(0);
  }
  return 0;
}
