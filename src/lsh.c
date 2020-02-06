#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/wait.h>
#include "./lsh.h"
#include "./constants.h"
#include "./command_things.h"
#include "./builtin.h"
#include "./feature.h"


extern const char* builtin_str[];
extern int (*builtin_func[]) (char**);

extern int (*feature_func[]) (char**, int*, ssize_t*);

char* lsh_get_command()
{
	char c;
	int buf_size = MAX_COMMAND_SIZE;
	int command_case_res;

	char* command = NULL; 
	ssize_t position = 0;
	
	command = malloc(sizeof(char) * buf_size);
	if (command == NULL) {
		perror("command malloc() failed.");
		exit(EXIT_FAILURE);
	}
	
	while (TRUE) {
		c = getchar();
		command_case_res = command_get_case(c);

		if (command_case_res == END_OF_FILE) {
			command[position] = '\0';
			return command;
		} else if (command_case_res == DEFAULT) {
			command[position++] = c;
		} else {
			// TODO: ex) single quotes, double quotes
			feature_func[command_case_res](&command, &buf_size, &position);
		}

		command_append(&command, &buf_size, &position);
	}
}

char** lsh_split_command(char* line)
{
	static const char* delim = " \t\r\n"; 	
	char* token;
	char** tokens;
	int buf_size = LSH_TOK_BUFSIZE, position = 0;

	tokens = malloc(buf_size * sizeof(char*));
	if (tokens == NULL) {
		perror("allocation error.");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, delim);
	while(token != NULL) {
		// printf("%s\n", token);
		tokens[position++] = token;		

		if (position >= buf_size) {
			buf_size += LSH_TOK_BUFSIZE;	
			tokens = realloc(tokens, buf_size * sizeof(char*));			
			if (tokens == NULL) {
				perror("reallocation error.");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	// if no more tokens are found, strtok() returns NULL

	tokens[position] = NULL;
	
	return tokens;
}

int lsh_launch(char** args)
{
	int status;
	pid_t pid;

	
	// if nothing was entered, then just return 1 to continue
	if (args[0] == NULL) {
		return 1;
	}

	pid = fork();
	if (pid  == 0) { // child
		if ((status = execvp(args[0], args)) == -1) {
			fprintf(stderr, "execvp() failed.\n");
			return 0;
		}
	} else if(pid < 0) { // error
		fprintf(stderr, "fork() error.");
		return 0;
	} else { // parent
		waitpid(pid, &status, 0);	

		if (!WIFEXITED(status)) {
			printf("exited unnormally, status=%d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("killed by signal %d\n", WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf("stopped by signal %d\n", WSTOPSIG(status));
		} else if (WIFCONTINUED(status)) {
			printf("continued\n");
		}
	}

	return 1;
}

int lsh_execute(char** args)
{
	int i, builtin_number;

	if (args[0] == NULL) return 1;

	builtin_number = builtin_num();
	for (i = 0; i < builtin_number; ++i) {
		if (strcmp(args[0], builtin_str[i]) == 0) // builtin.h
			return builtin_func[i](args);
	}

	// if args is not builtin then launch program(args)
	return lsh_launch(args);		
}

void lsh_loop()
{
	char* line;
	char** args; 
	int status;

	do {
		printf("> ");
		line = lsh_get_command();
		args = lsh_split_command(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	} while(status);
}
