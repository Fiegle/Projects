//COP4610
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>	
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* getAbsolutePath(char* path, char* cwd);
char* resolveDots(char* path);
void execute(char **cmd, char * cwd, int numcmd);
char* resolvePath(char* cmd);
int redirection(char** cmd, int numcmd, char **infile, char **outfile, char* cwd);
int pipestatus (char **cmd, char* cwd, int numcmd, int *ins2, int *ins3);
char** splitcmds(char **cmd,  int ins);
int checkargs(char** cmd, int cmdnum);

int main() {
	char* user = getenv("USER");
	char* machine = getenv("MACHINE");

	char* token = NULL;
	char* temp = NULL;
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	char *value;
	char cwd[100];
	while (1) {
		//get cwd and prompt
		getcwd(cwd, sizeof(cwd));
		//long prompt
		printf("%s@%s:%s>", user, machine, cwd);
		
		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

			int i;
			int start = 0;
			for (i = 0; i < strlen(token); i++) {
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
					if (i-start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr,specialChar);

					start = i + 1;
				}
			}

			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());    //until end of line is reached

		addNull(&instr);
		//printTokens(&instr);
		
//OUR CODE IS STARTING HERE//

		//check for exit
		if (strcmp(instr.tokens[0], "exit") == 0)
		// if there are no jobs then break, otherwise continue
			break;

		//expand and update tokens that are environmental variables
		int i,j;
		instruction *instr_ptr = &instr;
		for(i = 0; i< instr_ptr->numTokens; i++)
		{
			if((instr_ptr->tokens)[i] != NULL){
				if(instr.tokens[i][0] == '$'){
					//memmove(instr.tokens[i], instr.tokens[i]+1, strlen(instr.tokens[i]));
					char *name = getenv(instr.tokens[i]+1);
					if(name != NULL){	
						instr.tokens[i] = (char*)realloc(instr.tokens[i], strlen(name)+1);
						strcpy(instr.tokens[i], name);
					}
				}
			}
		
		}//done with env variables
	
	//built in determinationi, if built in expand arguments but not command
	if (strcmp(instr.tokens[0], "cd") == 0){
		//cd function
		if(instr.numTokens == 3){
                        instr.tokens[1] = getAbsolutePath(instr.tokens[1],cwd);
                        instr.tokens[1] = resolveDots(instr.tokens[1]);
                        chdir(instr.tokens[1]);
                }else if(instr.numTokens == 2){
                	chdir(getAbsolutePath("~",cwd));
                }else{
                	printf("cd: Too many arguments.\n");
                }
	}else if(strcmp(instr.tokens[0], "echo") == 0){
		//echo things
		int j,k,echoerr;
                for(k = 0; k < instr.numTokens-1; k++){
                        if(instr.tokens[k][0] == '$'){
                        printf("%s: Undefined variable.\n", instr.tokens[k]);
                        echoerr=1;
                        }
                }
                if(echoerr != 1){
                        for(j = 1; j < instr.numTokens-1; j++){
                                printf("%s ",instr.tokens[j]);

                        }
                }
        	printf("\n");
	}else if(strcmp(instr.tokens[0], "jobs") == 0){ 
		printf("There are No Jobs\n");
	}else{

		int argstatus = 0;
		//check for syntax errors in command
		argstatus  = checkargs(instr.tokens, instr.numTokens);
		
		//not a built in, look on path and execute, not expandng arguments
		if (argstatus == 0){
			instr.tokens[0] = resolvePath(instr.tokens[0]);
			execute(instr.tokens, cwd, instr.numTokens);
		}
	}
		//end of call, clear and start again
		clearInstruction(&instr);
	
	
	}//endof while 1
	//exit procedure
	clearInstruction(&instr);
	return 0;
}

//ensures arguments will act correctly, no errors in command
//as specified in project writeup
int checkargs(char** cmd, int cmdnum){
	//parse args for errors
	int x;	
	int i = 0;
	if (strcmp(cmd[0], "<") == 0 || strcmp(cmd[0], ">") == 0){
		printf("error: input or output symbol as initial token\n");
		return -1;
	}
	if (strcmp(cmd[0], "|") == 0){
		printf("pipe as inital token error");
		return -1;
	}
	for (i = 0; i < cmdnum-1; i++){
		if (strcmp(cmd[i], "<") == 0){
			if (cmd[i+1] == NULL){
				printf("error: no file for input\n");
				return -1;
			}
		}
	
		if (strcmp(cmd[i], ">") == 0){
			if (cmd[i+1] == NULL){
				printf("error: no file for output\n");
				return -1;
			}
		}
		if (strcmp(cmd[i], "|") == 0){
			if (cmd[i+1] == NULL){
				printf("error: no pipe destination\n");
				return -1;
			}
		}
	}
	return 0;
}

//when given a command, this function will on the path for the first
// runnable file and return the absolute path to it
char* resolvePath(char* cmd){
	char* pt = getenv("PATH");
	char* path = (char*)malloc(strlen(pt));
	strcpy(path, pt);
	char *p = strtok(path, ":");
	char **arr = (char**)malloc(sizeof(char*)+strlen(cmd)+1);
	int nump = 0;
	int i = 0;
	char* slash = "/";
	while (p != NULL){
		arr = (char**)realloc(arr, sizeof(char*)*(nump+2));
		arr[i] = (char*)malloc(strlen(p)+1);	
		strcpy(arr[i], p);
		strcat(arr[i], slash);
		strcat(arr[i], cmd);
		nump++;
		i++;
		p = strtok(NULL, ":");
	}
	arr[nump] = (char*)NULL;
	
	char* workdir = getenv("PWD");
		
	for (i = 0; i<nump; i++){
		if ((arr[i][0]) == '.'){
			arr[i] = getAbsolutePath(arr[i], workdir);
			arr[i] = resolveDots(arr[i]);			
		}
	}
	char* actual;
	//try to run
	for (i = 0; i<nump; i++){
		if (access(arr[i], X_OK) == 0){
			actual = (char*)malloc(strlen(arr[i])+1);
			strcpy(actual, arr[i]);
			break;
		}
	}
	for (i = 0; i<nump; i++){
		free(arr[i]);
	}
	//free(path);
	free(arr);
	return actual;

}

//handles redirection, parses arguments and creates necessary files
int redirection(char** cmd, int numcmd, char **infile, char ** outfile, char* cwd){
	int value = 0;
	int i = 0;
	for (i = 0; i < numcmd-1; i++){
		if (strcmp(cmd[i], "<")==0){
			//input redirect
			value = 1;
			*infile = cmd[i+1];
			*infile = getAbsolutePath(*infile, cwd);
			cmd[i] = NULL;
			break;
		}
	}
	for (i = 0; i < numcmd-1; i++){	
		if (cmd[i] != NULL){
			if (strcmp(cmd[i], ">") ==0){	
				//output redirection
				if (value == 1)
					value = 3;
				else
					value = 2;
				*outfile = cmd[i+1];
				*outfile = getAbsolutePath(*outfile, cwd);
				cmd[i] = NULL;
				break;
			}
		}
	}
	//1 for input redirect, 2 for output redirect,3 for both. 0 for none	
	return value;
}

//finds pipes and will give appropriate values when pipes occur
//also generates values to use in cmd splitting utility function to
//ease pipe implementation in execute() function
int piping (char **cmd, char* cwd, int numcmd, int *ins2, int *ins3){
	int status = 0;
	int i = 0;
	for ( i = 0; i < numcmd-2; i++ ){
		if (strcmp(cmd[i], "|") == 0){
			status += 1;
			cmd[i] = NULL;
			if (status == 1)
				*ins2 = i;
			else
				*ins3 = i;
	
		}
	}	
	//0 for none, 1 for 1, 2 for 2, -1 if invalid
	if (status >= 3)
		status = -1;
	return status;
}

//utility funciton: splits one command into multiple with second starting 
//at the ins+1 variable, assumes cmd[ins] is null 
char** splitcmds(char **cmd, int ins){
	//finds first null, in cmd, skips null, stores all tokens until next null into cmd2
	int i;
	char** cmd2;
	int count = 0;
	cmd2 = (char**)malloc(sizeof(char*));
	for (i = ins +1; cmd[i] != NULL; i++){
		cmd2 = (char**)realloc(cmd2, sizeof(char*)*(count+2));
		cmd2[count] = malloc(strlen(cmd[i]+1));
		strcpy(cmd2[count], cmd[i]);
		count++;
	} 
	cmd2[count] = NULL;
	return cmd2;
}

//function for executing commands, will only be called in case of valid call
//will determine redirection and pipe status from other functions  and act accordingly
void execute(char **cmd, char* cwd, int numcmd){
	int status;
	int pipestatus = 0;
	int redirect = 0;
	char *infile;
	char *outfile;
	int ins2, ins3; 
		
	redirect = redirection(cmd, numcmd, &infile, &outfile, cwd);
	if (redirect == 0){
		pipestatus = piping(cmd, cwd, numcmd, &ins2, &ins3);
	}
 
	int x = 0;
	pid_t pid = fork();
	if (pid == -1){
		printf("fork failed\n");
		exit(1);
	}else if (pid == 0){
		//child execute
		if (pipestatus == 1 || pipestatus == 2 ){
			char **cmd2;
			cmd2 = splitcmds(cmd, ins2);
			//now second command is in cmd2
			//and third co mmand is in cmd3 if there is one
			cmd2[0] = resolvePath(cmd2[0]);			
			pid_t pid2;
			int pipefd[2];
			int ret;
			if(pipestatus == 1){
			ret = pipe(pipefd);

			if(ret == -1){
				perror("piping error\n");
				exit(1);
			}
			pid2 = fork();
			if(pid2 == 0){
			close(pipefd[1]);
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
			execv(cmd2[0], cmd2);			
			}
			else{
				//if(fork()==0){
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
				execv(cmd[0], cmd);
				//}
				close(pipefd[0]);
				close(pipefd[1]);
				waitpid(-1, NULL, 0);
				}
			}else if(pipestatus == 2){
				char **cmd3;
				char **cmd4;
                                cmd3 = splitcmds(cmd, ins2);
				cmd4 = splitcmds(cmd,ins3);
				cmd3[0] = resolvePath(cmd3[0]);
				cmd4[0] = resolvePath(cmd4[0]);
				int piper[2];
				int piper2[2];
				pid_t pid3, pid4;
				pipe(piper);
				if((pid3 == fork()) == -1){
					printf("piping error\n");
					exit(1);
				} else if (pid3 == 0){
					pipe(piper2);
					if(pipe(piper2) == -1){
						perror("piping error\n");
						exit(1);
					}
					pid4 = fork();
					if(pid4 == -1){
						perror("fork error\n");
						exit(1);
					}
					else if(pid4 == 0){
						close(piper[1]);
						close(piper[0]);
						close(piper2[1]);
						dup2(piper2[0],0);
						close(piper2[0]);
						execv(cmd4[0], cmd4);
					}
					else{
						close(piper[1]);
						dup2(piper[0],0);
						close(piper[0]);
						close(piper2[0]);
						dup2(piper2[1],1);
						close(piper2[1]);
						execv(cmd3[0], cmd3);
					}
				
				}
				else{
					close(piper[0]);
					dup2(piper[1],1);
					close(piper[1]);
					execv(cmd[0], cmd);
				}
			}
		}
		if (redirect == 2 || redirect == 3){
			//output
			int fd;
			fd = (open(outfile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR));
			dup2(fd,1);
			close(fd);
		}
		if (redirect == 1 || redirect == 3){
			//input
			int fd;
			fd = (open(infile, O_RDONLY));
			dup2(fd,0);
			close(fd);

		}
		if (pipestatus == 0){
			execv(cmd[0], cmd);
                        printf("problem executing %s\n", cmd[0]);
                        exit(1);
		}

	}else{
		//parent execute
		waitpid(pid, &status, 0);
	}
}

//calls functions in pathutil.h which correct . and .. in paths
char* resolveDots(char* path){
	while(strstr(path,"..") != NULL){
		path = (char*)resolveDotDot(path);
	}
	while (strstr(path, ".") != NULL){
		path = (char*)resolveDot(path);
	}
	return path;
}

//gets a path and converts it to absolute path
char* getAbsolutePath(char* path, char* cwd){
	if (path[0] == '/')//begins with /, already absolute path	
		return path;
	char* rootpath = getenv("HOME");
	if (path[0] == '~'){//beings with ~, concatenate home to front
		char* temp = (char*) malloc(strlen(rootpath) + strlen(path));
		strcpy(temp, rootpath);
		strcat(temp, path+1);
		return temp;
	}//normal path, convert to absolute by adding pwd
	//char* currentdir = getenv("PWD");
	char*temp = (char*) malloc(strlen(cwd) + strlen(path) + 1);
	strcpy(temp, cwd);
	strcat(temp, "/");
	strcat(temp, path);
	return temp;	
}

//OUR CODE ENDS HERE

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
	instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++) {
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++){
		free(instr_ptr->tokens[i]);	
	}
	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}
