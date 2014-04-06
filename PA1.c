#include "gradefile.h"
#include "compile.h"
#include "source_check.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define PA1

int make_gradefile(char* comments, char* name, int comp_score, char* grader_name, char* test_out, char* lateness)
{
    char* p = "            CS-2031 System Programming for Non-Majors\n"
              "                          D-term 2014\n"
              "                   Programming Assignment 1:\n"
              "\n"
              "Name:  \n"
              "Student:  %s\n"
              "\n"
              "Requirement                                              Points\n"
              "----------------------------------------------------------------------\n"
              "Correct compilation without warnings                        %i/2\n"
              "Correct execution of TA test cases                          #/2\n"
              "Correct usage of scanf() to read six inputs                 #/6\n"
              "Correct usage of printf() to print outputs                  #/5\n"
              "\n"
              "----------------------------------------------------------------------\n"
              "Subtotal:                                                  ##/15\n"
              "Late Penalty:                                               %s\n"
              "\n"
              "Final Grade = #\n"
              "\n"
              "----------------------------------------------------------------------\n"
              "TA Comments:\n"
              "    Test Case: 3-4-5 triangle:\n\n%s\n"
              "\n"
              "\n"
              "Graded by: %s";

    char sheet[4096];
    char comment_file[1024];

    snprintf(sheet, 4096, p, name, comp_score, lateness, test_out, grader_name);
    snprintf(comment_file, 1024, "%s/grade.txt", comments);

    FILE *f = fopen(comment_file, "w");
    if (f == NULL)
    {
        printf("Cannot Write to File!");
    }
    else
    {
        fprintf(f, sheet);
        fclose(f);
    }
    
}




// return the numbe of points to assign on this assignment (0 or 2)
int compile_code(char* source, char* name)
{
    char exec[1024];
    char errorfile[1024];
    snprintf(errorfile, 1024, "./students/%s/errors.txt", name);
    snprintf(exec, 1024, "gcc -lm -Wall -o %s.exec %s > /dev/null 2>%s", name, source, errorfile);
    system(exec);
    return (1-file_exists(errorfile))*2;
}



int source_match(char* name)
{
    char* ext = strrchr(name, '.');
    return ext&&(*(ext+1))=='c'
}


char* run_code(char* username)
{
    char exec[1024];
    snprintf(exec, 1024, "echo \"0 0 3 0 0 4\n\" | ./%s.exec > .out 2>&1", username);
    system(exec);

    char *file_contents;
    long input_file_size;

    FILE *input_file = fopen(".out", "rb");

    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);

    rewind(input_file);
    file_contents = malloc(input_file_size * (sizeof(char)));
    fread(file_contents, sizeof(char), input_file_size, input_file);

    fclose(input_file);

    return file_contents;
}
