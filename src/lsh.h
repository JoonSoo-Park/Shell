#ifndef LSH_H
#define LSH_H


char* lsh_get_command();
char** lsh_split_command(char*);
int lsh_execute(char**);
void lsh_loop();

#endif
