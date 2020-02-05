#ifndef LSH_H
#define LSH_H

int lsh_cd(char**);
int lsh_help(char**);
int lsh_exit(char**);

char* lsh_get_command();
char** lsh_split_command(char*);
int lsh_execute(char**);
void lsh_loop();

#endif
