/**
 * @ Author: Your name
 * @ Create Time: 2024-03-11 12:08:01
 * @ Modified by: Your name
 * @ Modified time: 2024-03-12 14:18:56
 * @ Description:
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include "pstree.h"
#include <stdlib.h>
#include "khash.h"

KHASH_MAP_INIT_INT(ProcessNode_map,ProcessNode *);

int main(int argc, char const *argv[]) {
    bool print_pid = false;  // the boolean type is not built-in in C. it's included from stdbool.h
    bool sort_pid = false;
    bool print_version = false;
    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];
        assert(arg);
        printf("argv[%d] = %s\n", i, arg);
        if (strcmp(arg, "-p")==0 || strcmp(arg, "--show-pids")==0) {  // if equal, strcmp returns 0
            print_pid = true;
        } else if (strcmp(arg, "-n")==0 || strcmp(arg, "--numeric-sort")==0) {
            sort_pid = true;
        } else if (strcmp(arg, "-V")==0 || strcmp(arg, "--version")==0) {
            print_version = true;
        }

    }

    assert(!argv[argc]);

    DIR *dir;
    const char *basePath = "/proc";
    struct dirent *entry;
    if ((dir = opendir(basePath)) == NULL) {
        perror("error opening directory");
        return 1;
    }
    // create a process struct
    ProcessNode* dummyhead = malloc(sizeof(ProcessNode));

    ProcessNode* curr = malloc(sizeof(ProcessNode));
    dummyhead->next = curr;
    curr->prev = dummyhead;

    khint_t k;
    khash_t(ProcessNode_map) *h = kh_init(ProcessNode_map);

    while ((entry = readdir(dir)) != NULL) {
        pid_t pid = 0;
        if ((pid = is_number(
                entry->d_name))) { // check if the dir is a valid int representing pid, as well as assigning it to the pid variable

            char filename[50]; //fixed size for now

            sprintf(filename, "/proc/%d/status", pid);
            FILE *statusFp = fopen(filename, "r");
            if (statusFp == NULL) {
                char errorMessage[50];
                sprintf(errorMessage, "Error opening file, pid: %d", pid);
                perror(errorMessage);
                return 1;
            }

            // read the status file, extracting information required
            char buffer[100];
            curr->pid = pid;
            k = kh_put(ProcessNode_map, h, pid, curr);

            while (fgets(buffer, sizeof(buffer), statusFp) != NULL){
                // printf("%s", buffer);
                if (start_with(buffer,"Name")){
                    get_content(buffer,curr->name);
                }else if (start_with(buffer,"PPid")){
                    char content[20];
                    get_content(buffer,content);
                    curr->parent_id = atoi(content);
                }
                memset(buffer, 0, sizeof(buffer));
            }

            fclose(statusFp);
            // ProcessNode * newProc = malloc(sizeof(ProcessNode));
            // prev->next = newProc;
            // newProc->prev = prev;
            // prev = newProc;

            pid_t ppid = curr->parent_id;
            k = kh_get(ProcessNode_map,h,ppid);
            printf("%d\n", k);

            if (k != kh_end(h)){
                ProcessNode* parent = kh_value(h,k);
                ProcessNode * firstChild = parent->childhead;
                if (firstChild!=NULL){
                    // insert the new node into the parent's children
                    // prev->prev = parent;
                    curr->next = firstChild;
                    parent->childhead = curr;
                    firstChild->prev = curr;
                }else{
                    // if the parent has no child yet, attach curr to its childhead
                    parent->childhead = curr;
                }
            }
            curr = malloc(sizeof(ProcessNode));

        }
    }

    return 0;
}


pid_t is_number(char *str) {
    pid_t pid = 0;
    while (*str) {
        if (!isdigit(*str)) {
            return 0; // invalid pid. the dir is likely not a number
        }
        pid = pid * 10 + (*str - '0');
        str++;
    }

    return pid; // Valid number
}


int start_with(char* str,char* prefix){
    int commonIndex = 0;
    while (*str)
    {
        if (commonIndex > strlen(prefix) - 1){
            break;
        }
        if (commonIndex > strlen(str) - 1)
        {
            return 0;
        }
        if (str[commonIndex]!=prefix[commonIndex]){
            return 0;
        }
        commonIndex++;
    }
    return 1;
}

void get_content(char*str,char* dest){
    int length = strlen(str);
    int index = length-2;
    char c;
    while (index>=0 &&( c = str[index]) && c!='n' && c!='\t' && c!=' ')
    {
        index--;
    }
    for (int i = 0;i<length-2-index;i++){
        dest[i] = str[index+i+1];
    }
    dest[length-2-index] = '\0';
}
 