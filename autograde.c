#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

char* get_source_file(char* indir);
int compile_code(char* source, char* name);
int make_gradefile(char* comments, char* name, int comp_score, char* grader_name, char* test_out, char* lateness);
char* run_code(char* username);
int file_exists(char* path);

int main()
{

    DIR* pwd;
    DIR* fid;
    DIR* fod;

    struct dirent *dir;
    char cwd[1024];

    // attempt to get the current directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("error: cannot access the cwd");
        return 1;
    }


    char rf[3] = {0};

    pwd = opendir(cwd);
    if (pwd)
    {
        while ((dir = readdir(pwd)) != NULL)
        {
            char* name = dir->d_name;
            if (strcmp(name, "COMMENTS") == 0)
            {
                rf[0] = 1;
            }
            else if (strcmp(name, "groups") == 0)
            {
                rf[1] = 1;
            }
            else if (strcmp(name, "students") == 0)
            {
                rf[2] = 1;
            }
        }
        closedir(pwd);

        if (!(rf[0] && rf[1] && rf[2]))
        {
            perror("you are not in a grades directory. please make sure that you have COMMENTS, groups, and students as sub-dirs.");
            return 1;
        }
    }
    else
    {
        perror("error: cannot read the cwd");
        return 1;
    }



    printf("Directory structure tests passed...\nBeginning grading...\nonly showing failed tests...\n");

    char students[1024];
    char comments[1024];
    char ca_student[1024];
    char cc_student[1024];
    char cl_student[1024];

    snprintf(students, sizeof students, "%s/%s", cwd, "students");
    snprintf(comments, sizeof comments, "%s/%s", cwd, "COMMENTS");

    
    pwd = opendir(students);
    if (pwd)
    {
        while ((dir = readdir(pwd)) != NULL)
        {
            char* name = dir->d_name;
            if (*name != '.') // dont work on dotfiles plz
            {
                //printf("%s\n", name);
                snprintf(ca_student, sizeof ca_student, "%s/%s", students, name);
                snprintf(cc_student, sizeof cc_student, "%s/%s", comments, name);
                snprintf(cl_student, sizeof cc_student, "%s/%s/%s", students, name, "LATE-SUBMISSION-FLAG.txt");
                char* source = get_source_file(ca_student);
                int comp_score = 0;
                char* late_penalty = "0";
                if (file_exists(cl_student))
                {
                    printf("%s is LATE\n", name);
                    late_penalty = "LATE";
                }

                if (source)
                {
                    comp_score = compile_code(source, name);
                    if (comp_score == 0)
                    printf("compile grade for: %s ==> %i/2\n", name, comp_score);
                }
                else
                {
                    printf("compile grade for: %s ==> ERROR\n", name);
                }
                free(source);
                
                char* run_result = run_code(name);

                char* grader = getenv("GRADER");
                if (!grader)
                {
                    grader = "a nonny moose";
                }
                make_gradefile(cc_student, name, comp_score, grader, run_result, late_penalty);
                free(run_result);

            }
        }
        closedir(pwd);
    }
    else
    {
        perror("error: cannot read student's assignments");
        return 1;
    }
}


char* get_source_file(char* indir)
{
    DIR* pwd;
    struct dirent *dir;
    char* fname = (char*)(malloc(1024));
    pwd = opendir(indir);
    if (pwd)
    {
        while ((dir = readdir(pwd)) != NULL)
        {
            char* name = dir->d_name;
            if (*name != '.') // dont work on dotfiles plz
            {
                char* ext = strrchr(name, '.');
                if (ext && (*(ext+1))=='c')
                {
                    snprintf(fname, 1024, "%s/%s", indir, name);
                    closedir(pwd);
                    return fname;
                }
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

// return the numbe of points to assign on this assignment (0 or 2)
int compile_code(char* source, char* name)
{
    char exec[1024];
    snprintf(exec, 1024, "gcc -lm -o %s.exec %s > /dev/null 2>./.errors", name, source);
    system(exec);
    return (1-file_exists("./.errors"))*2;
}


int file_exists(char* path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        return 0; //uh oh
    }
    else
    {
        char x;
        if (fscanf(fp, "%c", &x) == EOF)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        fclose(fp);
    }
}


char* run_code(char* username)
{
    //return "code result will be here!";
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
              "    Test Case: 3-4-5 triangle:\n%s\n"
              "\n"
              "\n"
              "Graded by: %s\0";

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