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
    char* p = "                       Project 3 -- User-Designed Functions\n"
              "                                CS 2301 D-term 2014\n"
              "\n"
              "Student name:	\n"
              "   E-mail ID: \n"
              "\n"
              "-----------------------------------------------------------------------\n"
              "  Correct compilation without warnings	 __x/2__\n"
              "\n"
              "  Satisfactory README file	 __x/3__\n"
              "    - 1 point: No instructions on how to run the program.\n"
              "\n"
              "  Pre- and post-conditions specified as comments in __x/5__\n"
              "all functions\n"
              "\n"
              "  Correct execution with graders’ test cases	 __x/5__\n"
              "\n"
              "  Ability to recover from invalid inputs	         __x/2__ \n"
              "\n"
              "  Function definitions, parameters and return types __x/4__\n"
              "according to specifications (1 point each)\n"
              "\n"
              "  Correct implementation of functions	 __x/4__\n"
              "\n"
              "------------------------------------------------------------------------\n"
              "SubTotal:                               _xx/25_\n"
              "\n"
              "Late Penalty:	(5 points per day, one day max)	 ___/-5	\n"
              "Note here if Grace Day is used	 ___\n"
              "\n"
              "Unclean project: Penalty for submitting a project that\n"
              "has not been cleaned	 ___/-5\n"
              "\n"
              "------------------------------------------------------------------------\n"
              "Total:                                  _xx/25_\n"
              "\n"
              "\n"
              "\n"
              "\n"
              "Graded by %s\n"
              "\n"
              "------------------------------------------------------------------------\n"
              "Comments:\n"
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