#include "./command_things.h"
#include "./constants.h"
#include <stdio.h>
#include <stdlib.h>


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
		*command = (char*)realloc(*command, *buf_size);
		if (*command == NULL) {
			perror("Command realloc() failed.");
			exit(EXIT_FAILURE);
		}
	}

	return 1;
}
