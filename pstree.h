//
// Created by linga on 2024/3/11.
//

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>

#ifndef OS_WORKBENCH_PSTREE_H
#define OS_WORKBENCH_PSTREE_H

#endif //OS_WORKBENCH_PSTREE_H

pid_t is_number(char* str);
void get_content(char *str, char *dest);
int start_with(char *str, char *prefix) ;

// typedef struct Process Process;
// struct Process{
//     pid_t pid;
//     char name[50];
//     pid_t parent_id;
// };

typedef struct ProcessNode ProcessNode; // Forward declaration of struct Process

struct ProcessNode
{
    pid_t pid;
    char name[50];
    pid_t parent_id;
    ProcessNode *next; // Use the completed type Process*
    ProcessNode *prev; // Use the completed type Process*
    ProcessNode *childhead // ptr to the head, of the linked list of all its children
};


