#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

char* lsh_get_command();
char** lsh_split_command(char*);
int lsh_execute(char**);
void lsh_loop();

int lsh_cd(char**);
int lsh_help(char**);
int lsh_exit(char**);

#define MAX_COMMAND_SIZE 5
#define LSH_TOK_BUFSIZE 2

enum COMMAND_GET_CASE {
	END_OF_FILE,
	DEFAULT,
	ESCAPE_CHARACTER,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
};

char* builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char**) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};

int builtin_num()
{
	return sizeof(builtin_str) / sizeof(char*);
}

int lsh_cd(char** args)
{
	if (args[0] == NULL) {
		fprintf(stderr, "usage: cd [arguments]");
	} else {
		if (chdir(args[1]) == -1) {
			perror("change directory failed.");
		}
	}
	return 1;
}

int lsh_help(char** args)
{
	int i;

	printf("These shell commands are defined internally.\n");
	for (i = 0; i < builtin_num(); ++i) {
		printf("%s\n", builtin_str[i]);
	}

	return 1;
}

int lsh_exit(char** args)
{
	return 0;
}

int command_get_case(char c)
{
	if (c == EOF || c == '\n') {
		return END_OF_FILE;
	}
	else if (c == '\\') {
		return ESCAPE_CHARACTER;
	}
	else if (c == '\'') {
		return SINGLE_QUOTE; 
	}
	else if (c == '\"') {
		return DOUBLE_QUOTE;
	}
	else {
		return DEFAULT;
	}
}

int command_append(char** command, int *buf_size, ssize_t *position)
{
	if (*position >= *buf_size) {
		*buf_size += MAX_COMMAND_SIZE;
		*command = realloc(*command, *buf_size);
		if (*command == NULL) {
			perror("Command realloc() failed.");
			exit(EXIT_FAILURE);
		}
		printf("realloc size : %d\n", *buf_size);
	}

	return 1;
}

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

		}

		command_append(&command, &buf_size, &position);
	}

	// By using getine, it's harder to check if command has \ or \n
	/*
	if ((position = getline(&command, &size, stdin)) != -1) {
		command[position] = '\n';
		return command;
	} else {
		perror("Error getline().");
		exit(EXIT_FAILURE);
	}
	*/
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
		if (strcmp(args[0], builtin_str[i]) == 0)
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
		// TODO: remove all backslash from args.
		status = lsh_execute(args);

		free(line);
		free(args);
	} while(status);
}

int main(int argc, char *argv[])
{
	lsh_loop();

	return 0;
}
