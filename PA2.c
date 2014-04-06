#include "gradefile.h"
#include "compile.h"
#include "source_check.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PA2
#define COMMENT_SIZE 16384

int copy_all_execs(char* student);

int make_gradefile(char* comments, char* name, int comp_score, char* grader_name, char* test_out, char* lateness)
{
    char* p = "            CS-2031 System Programming for Non-Majors\n"
              "                          D-term 2014\n"
              "                   Programming Assignment 2:\n"
              "\n"
              "Name:  \n"
              "Student:  %s\n"
              "\n"
              "-----------------------------------------------------------------------\n"
              "  Compilation without warnings using make and -Wall   %i/2__\n"
              "\n"
              "  Satisfactory README file describing describing program and extra __/3__\n"
              "credit part (if any)\n"
              "\n"
              "  Correct execution with graders' test cases  __/5__\n"
              "(both leap year and non-leap year, various starting days)\n"
              "\n"
              "  Correct use of scanf() to get inputs  __/1__\n"
              "\n"
              "  Correct use of printf() to print lines of calendar  __/3__\n"
              "\n"
              "  Correctly usage of conditional and loop statements  __/6__\n"
              "\n"
              "\n"
              "------------------------------------------------------------------------\n"
              "SubTotal:                                       __/20_\n"
              "\n"
              "\n"
              "Extra credit:\n"
              "\n"
              "  Accept year and calculate starting day and leap year  __/5__\n"
              "     (submitted as a separate Calendar2.c file)\n"
              "\n"
              "\n"
              "Late Penalty:  (5 points per day (i.e., 25%), one day max)   __/-5__\n"
              "Check here if Grace Day applied  %s\n"
              "(Grace Day does not apply to programs that failed to compile)\n"
              "\n"
              "Reason for grade of zero (if applicable):--\n"
              "  ____  no submission\n"
              "  ____  program fails to compile\n"
              "  ____  other (specify) _________________________________________\n"
              "\n"
              "------------------------------------------------------------------------\n"
              "Total:                                  __/20_\n"
              "\n"
              "\n"
              "Graded by %s\n"
              "------------------------------------------------------------------------\n"
              "Comments:\n"
              "\tTest Cases: %s\n\n"
              "\n";

    char sheet[COMMENT_SIZE];
    char comment_file[1024];

    snprintf(sheet, COMMENT_SIZE, p, name, comp_score, lateness, grader_name, test_out);
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
    snprintf(exec, 1024, "cd students/%s/ >>/dev/null && make > /dev/null 2>errors.txt", name, name, errorfile);
    system(exec);
    wait(500);
    copy_all_execs(name);
    return (1-file_exists(errorfile))*2;
}


int source_match(char* name)
{
    return strcmp(name,"makefile")==0;
}



int is_executable(char* path)
{
    return access(path, F_OK|X_OK) == 0;
}



int copy_all_execs(char* student)
{
    char indir[1024];
    char move[2048];
    char path[2048];
    snprintf(indir, 1024, "students/%s/", student);
    int exec_count = 0;


    DIR* pwd;
    struct dirent *dir;
    char* fname = (char*)(malloc(1024));
    pwd = opendir(indir);
    if (pwd)
    {
        while ((dir = readdir(pwd)) != NULL)
        {
            char* name = dir->d_name;
            snprintf(path, 1024, "students/%s/%s", student, name);

            if (*name=='.')
            {
                //printf("%s is a dot file!\n", name);
            }
            else if (!is_executable(path))
            {
                //printf("%s is not executable\n", name);
            }
            else
            {
                snprintf(move, 2048, "cp %s ./%s-%i.exec", path, student, exec_count);
                exec_count++;
                system(move);
            }
        }
        closedir(pwd);
        return 0;
    }
    else
    {
        perror("error: cannot read student's assignments");
        return 0;
    }
}



char* run_code(char* username)
{
    char exec[1024];
    snprintf(exec, 1024, "echo \"0 0 \n\" | ./%s-0.exec > .out 2>&1", username);
    system(exec);
    snprintf(exec, 1024, "echo \"1900 \n\" | ./%s-1.exec >> .out 2>&1", username);
    system(exec);

    wait(100);

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
