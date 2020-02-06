#include "./feature.h"

int (*feature_func[]) (char**, int*, ssize_t*) = {
	[2] = &escape_character
};

int escape_character(char** command, int *buf_size, ssize_t *position)
{
	char c;
	
	c = getchar();
	(*command)[(*position)++] = c;

	return 1;
}
