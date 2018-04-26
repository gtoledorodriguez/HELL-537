#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[]){
  // argv[0] -> my-zip
  if(argc >= 2){
    int i = 1;
    char letter = '\0';
    int letterNum = 0;
    while(i < argc){
     FILE *fp = fopen(argv[i],"r");
     if (fp == NULL) {
       printf("my-zip: cannot open file\n");
       exit(1);
     }
     char* buffer;
     size_t bufferSize = 1024;
    buffer = (char*) malloc(bufferSize * sizeof(char));
     while(getline(&buffer, &bufferSize, fp)!= -1){
         int m = 0;
           while(buffer[m] != '\0'){
             if(letter == '\0'){
		  letter = buffer[m];
                  letterNum++;
                  m++;
             }
	     else if(buffer[m] == letter){
                 letterNum++;
                 m++;
              }
             else{
	         fwrite(&letterNum, sizeof(int), 1, stdout);
	         printf("%c",letter);
                 letterNum = 0;
                 letter = '\0';
	      }
           }
     }
     free(buffer);
     fclose(fp);
     i++;
    }
    if(letter != '\0'){
	         fwrite(&letterNum, sizeof(int), 1, stdout);
	         printf("%c",letter);
                 letterNum = 0;
                 letter = '\0';
    }
  }
  else if(argc < 2){
     printf("my-zip: file1 [file2 ...]\n");
       exit(1);
  }
  else{
    exit(0);
  }
  return 0;
}
