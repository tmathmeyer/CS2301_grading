#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "gradefile.h"
#include "compile.h"
#include "source_check.h"

char* get_source_file(char* indir);
int file_exists(char* path);




int main()
{

    DIR* pwd;
    DIR* fid;
    DIR* fod;

    struct dirent *dir;
    char cwd[1024];

    char* grader = getenv("GRADER");
    if (!grader)
    {
        perror("error! please set the GRADER environment variable to your name!");
        return 1;
    }

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

                
                make_gradefile(cc_student, name, comp_score, grader, run_result, late_penalty);
                free(run_result);
            }
        }
        closedir(pwd);
        system("rm *.exec");
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
                if (source_match(name))
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

