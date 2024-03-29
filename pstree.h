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

#define PROGRAM_VERSION "1.0"

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

void print_tree(ProcessNode *root, int depth,bool print_pid);
void print_self_info(int depth, bool print_pid, ProcessNode *node);
void print_indentation(int depth);
void print_children_recur(ProcessNode *head, int depth, bool print_pid);
void print_tree_asc(ProcessNode *root, int depth, bool print_pid);
void print_tree(ProcessNode *root, int depth, bool print_pid);
