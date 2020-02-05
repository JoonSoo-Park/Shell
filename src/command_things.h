#ifndef COMMAND_THINGS_H
#define COMMAND_THINGS_H

#include <stdlib.h>

enum COMMAND_GET_CASE {
	END_OF_FILE,
	DEFAULT,
	ESCAPE_CHARACTER,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
};

int command_get_case(char);

int command_append(char**, int*, ssize_t*);

#endif
