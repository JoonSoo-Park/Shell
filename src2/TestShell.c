#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

// TODO: 
// 1. pipe

#define TRUE 1
#define FALSE 0

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char**) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};

int lsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char*);
}

int lsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("lsh");
		}
	}
	return 1;
}

int lsh_help(char **args)
{
	int i;
	printf("JoonSoo-Park Shell\n");
	printf("Type program names and arguments, and hit enter. \n");
	printf("The following are built in: \n");

	for (i = 0; i < lsh_num_builtins(); ++i) {
		printf("\t%s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int lsh_exit(char **args)
{
	printf("Goodbye\n");
	return 0;
}

int lsh_read_line(char** line)
{
	char *buf = NULL;

	buf = readline("> ");
	if (buf && *buf) {
		add_history(buf);
		*line = malloc(strlen(buf) + 1);
		strcpy(*line, buf);
		free(buf);
		return 1;
	}
	return 0;
}

char** lsh_split_line(char* line)
{
	int bufsize = LSH_TOK_BUFSIZE; 		
	int position = 0;
	char* token = NULL;
	char** tokens = NULL;
	
	tokens = malloc(bufsize * sizeof(char*));

	if (tokens == NULL) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position++] = token;

		if (position >= bufsize) {
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (tokens == NULL) {
				fprintf(stderr, "lsh: reallocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int lsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	printf("lsh_launch\n");

	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	} else {
		perror("lsh");
	}

	return 1;
}

/*
int lsh_launch_pipe(char** args, char** argsPipe)
{
	int pipefd[2];	
	pid_t p1, p2;

	printf("lsh_launch_pipe\n");

	if (pipe(pipefd) < 0) {
		fprintf(stderr, "Pipe could not be initialized");
		return 0;
	}

	p1 = fork();
	if (p1 < 0) {
		fprintf(stderr, "Fork failed");
		return 0;
	}
	if (p1 == 0) {
		printf("child1\n");
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execvp(args[0], args) < 0) {
			fprintf(stderr, "Could not execute command 1...");
			return 0;
		}
	} else {
		p2 = fork();
		if (p2 < 0) {
			fprintf(stderr, "Frok failed");
			return 0;
		}
		if (p2 == 0) {
			printf("child2\n");
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			printf("child2-2\n");

			if (execvp(argsPipe[0], argsPipe) < 0) {
				fprintf(stderr, "Could not execute command 2...");
				return 0;
			}
		}
		else {
			wait(NULL);
			wait(NULL);
		}
	}
}
*/

int lsh_launch_pipe(char** args, char** argsPipe)
{
	int pipefd[2];
	pipe(pipefd);
	int i;
	pid_t pid1, pid2;
	pid1 = fork();

	if (pid1 == -1) {
		char* error = strerror(errno);
		printf("error fork!!\n");
		return 0;
	}
	if (pid1 == 0) {
		close(pipefd[1]);
		dup2(pipefd[0], 0);
		close(pipefd[0]);
		execvp(argsPipe[0], argsPipe);
		char *error = strerror(errno);
		printf("unknown command\n");
		return 0;
	} else {
		pid2 = fork();
		
		if (pid2 == -1) {
			char* error = strerror(errno);
			printf("error fork!!\n");
			return 0;
		} 
		if (pid2 == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], 1);
			close(pipefd[1]);
			execvp(args[0], args);
			char *error = strerror(errno);
			printf("unknown command\n");
		}
		else {
			wait(NULL);
			wait(NULL);
		}
	}
}

int lsh_execute(char **args, char** argsPipe)
{
	int i;

	if (args[0] == NULL) {
		return 1;
	}

	for (i = 0; i < lsh_num_builtins(); ++i) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	// not builtin command, not piped
	if (argsPipe == NULL)
		return lsh_launch(args);
	else 
		return lsh_launch_pipe(args, argsPipe);
}

int lsh_split_pipe(char** line, char** pipe)
{
	// TODO:
	// if no input returns -1
	// if piped returns 1
	// if not piped return 0
	char* delim = "|";	

	strtok(*line, delim);
	*pipe = strtok(NULL, delim);

	if (*pipe == NULL)
		return 0;
	return 1;
}

void lsh_loop(void)
{
	// TODO
	// 1. read line
	// 2. check if it has pipe
	// 3. if it has pipe then seperate it.
	char *line, *pipe;
	char **args = NULL, **argsPipe = NULL;
	int status, piped;

	do {
		lsh_read_line(&line);

		piped = lsh_split_pipe(&line, &pipe);

		if (piped) {
			args = lsh_split_line(line);
			argsPipe = lsh_split_line(pipe);
		} else {
			args = lsh_split_line(line);
		}

		status = lsh_execute(args, argsPipe);

		free(line);
		free(args);
		//free(argsPipe);
	} while(status);
}

int main()
{
	lsh_loop();

	return EXIT_SUCCESS;
}
