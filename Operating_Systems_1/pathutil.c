
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include "pathutil.h"

char* resolveDot(char* path){

	char* occurence = strstr(path, "/.");
	char* actual = (char*)malloc(strlen(path));
	strcpy(actual, path);
	int x = strlen(path);
	int y = strlen(occurence);
	actual[x-y] = '\0';
	actual = strcat(actual, occurence+2);
	return actual;
}

char* resolveDotDot(char* path){
	
	char* occurence = strstr(path, "/..");
	char* actual = (char*)malloc(strlen(path));
	strcpy(actual, path);
	int x = strlen(path);
	int y = strlen(occurence);
	int i = 0;
	for (i = x - y - 1; i > -1; i--){
		if (actual[i] == '/'){
			actual[i] = '\0';
			i = -1;
		}
	}
	actual = strcat(actual, occurence+3);
	
	return actual;
}
