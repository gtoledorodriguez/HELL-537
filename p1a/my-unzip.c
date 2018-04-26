#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[]){
  // argv[0] -> my-unzip
  if(argc >= 2){
    int i = 1;
    char* letter = (char*)malloc(sizeof(char));
    int* number = (int*) malloc(sizeof(int));
    while(i < argc){
     FILE *fp = fopen(argv[i],"r");
     if (fp == NULL) {
       printf("my-unzip: cannot open file\n");
       exit(1);
     }
     int j;
     while(fread(number, sizeof(int), 1, fp) == 1){
	*letter = getc(fp);
        for( j = 0; j<*number; j++){
           printf("%s",letter);
        }
     }
     fclose(fp);
     i++;
    }
    free(letter);
    free(number);
  }
  else if(argc < 2){
     printf("my-unzip: file1 [file2 ...]\n");
       exit(1);
  }
  else{
    exit(0);
  }
  return 0;
}
