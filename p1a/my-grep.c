#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compareWords(char *bufferPos, char *word){
	int n = 0;
	while(*(word+ n) != '\0'){
          if(*(bufferPos + n) == *(word+ n)){
              n++;
          }
          else{
              return 0;
          }
     }
       return 1;
}

int main (int argc, char *argv[]){
  // argv[0] -> my-grep
  if(argc >=3){
     int i = 2;
     while(i <= argc -1){
     FILE *fp = fopen(argv[i],"r");
     if (fp == NULL) {
       printf("my-grep: cannot open file\n");
       exit(1);
     }
     char* buffer;
     size_t bufferSize = 512;
     buffer = (char*) malloc(bufferSize * sizeof(char));
     while(getline(&buffer, &bufferSize, fp)!= -1){
	int j =0;
       while(*(buffer+j) != '\0'){
	 if(compareWords((buffer+j), argv[1]) == 1){
       	 	printf("%s",buffer);
                break;
         }
         j++;
        }

     }  
     fclose(fp);
     i++;
     free(buffer);	
}
}
else if(argc == 2){
     char* buffer;
     size_t bufferSize = 512;
     buffer = (char*) malloc(bufferSize * sizeof(char));
     while(getline(&buffer, &bufferSize, stdin)!= -1){
	int j =0;
       while(*(buffer+j) != '\0'){
	 if(compareWords((buffer+j), argv[1]) == 1){
       	 	printf("%s",buffer);
         }
         j++;
        }	
     }  
     free(buffer);
}
 else{
  printf("my-grep: searchterm [file ...]\n");
  exit(1);
}
  return 0;
}


