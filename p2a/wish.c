#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define ARRAY_SIZE 512
#define DLMNT " \t\r\n\a"

const char error_message[30] = "An error has occurred\n";
const char prompt_message[] = "wish> ";
char *path[ARRAY_SIZE];
int batch = 0; 

void printPrompt(){
if (batch == 0){
    write(STDOUT_FILENO, prompt_message, strlen(prompt_message));
    fflush(stdout);
  }
}

void printError(){
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int tokenization(char *input, char *words[], char *deliminator){
  char *dummy;
  words[0] = strtok_r(input, deliminator, &dummy);
  int count = 1;
  while (1) {
	words[count] = strtok_r(NULL, deliminator, &dummy);
        if(words[count] == NULL)
		break; 
        count++;    
  }
  if(words[0] == NULL){
	return 0;
  }
  return count;
}

void internalCommand(char* input[], char*redirect_words[]){
	int child;
	int status;
	int found = 0;
	int temp = 0;
        char *realPath = (char *)malloc(ARRAY_SIZE * sizeof(char *));
	char *tempPath = (char *)malloc(ARRAY_SIZE * sizeof(char *));
	if(path[0] == NULL){
		printError();
		return;
	}
	if(input[0] == NULL)
		return;
	while(1){
		if(path[temp] == NULL)
			break;	
		if(strcpy(tempPath, path[temp]) == 0){
			printError();
			return;
		}
		tempPath[strlen(tempPath)] = '/';
		strcat(tempPath,input[0]);
		if(access(tempPath,X_OK) == 0){
			strcpy(realPath, tempPath);
			found = 1;
			break;
		}
		free(tempPath);
		temp++;
	}
	if(!found){
		printError();
		return;
	}
	child = fork();
	if(child == 0){
		if(redirect_words){
			int out = open(redirect_words[0], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
			if(out > 0){
				dup2(out,STDOUT_FILENO);
				fflush(stdout);
			}
		}
		execv(tempPath, input);
	}
	wait(&status);
}

void redirect(char *redir, char *cmd){
	char *preToken[ARRAY_SIZE];
	char *postToken[ARRAY_SIZE];
	redir[0] = '\0';
	redir++;
	if(tokenization(cmd, preToken, DLMNT) == 0){
		printError();
	 	return;
	}
	if(tokenization(redir, postToken, DLMNT) != 1){
		printError();
		return;
	}
	internalCommand(preToken, postToken);
}

int getCmd(char* input[],FILE * file){
    char* cmd = (char *)malloc(ARRAY_SIZE *sizeof(char));
    size_t length = ARRAY_SIZE;
    if (getline(&cmd, &length, file)== -1) {
        return 1;
    }
    if ((strcmp(cmd, "\n") == 0) || (strcmp(cmd, "") == 0))
        return -1;

    //delete '\n' at the end of the string
    if (cmd[strlen(cmd) - 1] == '\n')
        cmd[strlen(cmd) - 1] = '\0';
    if (cmd[0] == EOF){
        return 1;
    }

    //parallel
    char *parallel;
    if ((parallel = strchr(cmd, '&'))){
        char *totalCmd[ARRAY_SIZE];
	int numCmd = tokenization(cmd, totalCmd, "&");
	char* dummy_token[ARRAY_SIZE];
	char* redirect_token = (char *)malloc(ARRAY_SIZE * sizeof(char));
	for(int i = i; i<numCmd; i++){
		if((redirect_token = strchr(totalCmd[i], '>'))){
			redirect(redirect_token, totalCmd[i]);
			continue;
                }
		tokenization(totalCmd[i], dummy_token, DLMNT);
		internalCommand(dummy_token,NULL);
	}
        return -1;
    }

    // redirect
    char *redirect_cmd;
    if ((redirect_cmd = strchr(cmd, '>'))){
        redirect(redirect_cmd, cmd);
        return -1;
    }
    int word_count = tokenization(cmd, input, DLMNT);
    if (word_count == 0) {
        return 0;
    }

    // exit
    if (strcmp(input[0], "exit") == 0){
        if (input[1])
            printError();
        exit(0);
    }

    // cd
    if (strcmp(input[0], "cd") == 0){
	if (word_count != 2) {
           printError();
	   return 1;
        }
       if (chdir(input[1]) == -1){
          printError();
       }
        return -1; 
    }

    // path
    if (strcmp(input[0], "path") == 0){
        int temp = 0;
        while(1){
          if((path[temp] = input[temp+1]) == NULL)
            break;
        temp++;
       } 
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[]){
    path[0] = "/bin";
    FILE *file;
    char* input[ARRAY_SIZE];
   if(argc == 1){
	file = stdin;
    }
    else if (argc == 2) {
        if (!(file = fopen(argv[1], "r")) ){
            printError();
            exit(1);
        }
        batch = 1;
    }
    else{
        printError();
        exit(1);
    }
    while(1) {
            printPrompt();
 	    int cmdGot = getCmd(input, file);
            if (cmdGot == -1)
                continue;   
            if (cmdGot == 1)
                break; 
	    internalCommand(input, NULL); 
    }
    return 0;
}
