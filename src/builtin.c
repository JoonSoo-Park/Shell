#include "./builtin.h"
#include <stdio.h>
#include <unistd.h>

const char* builtin_str[] = {
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
