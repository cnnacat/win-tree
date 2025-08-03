#pragma once

#include <windows.h>
#include <wchar.h>
#include <stdbool.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <wchar.h>
#include <winerror.h>
#include <winnt.h>


typedef struct log_node 
{
    wchar_t*           message;
    struct log_node* next;
} log_node;

log_node*   init_log_node   (const wchar_t* message);
void        push_log_node   (log_node** head_log_node, log_node* this_log_node);
void        pop_log_node    (log_node** head_log_node);
void        output_log_file (log_node** head_log_node);
