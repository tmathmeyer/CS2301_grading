
#ifndef MAKE_GRADEFILE
#define MAKE_GRADEFILE

// separate the file saving from the other stuff
int make_gradefile(char* comments, char* name, int compile_score, char* grader_name, char* test_output, char* lateness);
char* run_code(char* username);

#endif