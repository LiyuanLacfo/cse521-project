#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (int);
void process_activate (void);
void unblock_parent(struct semaphore se);
char* get_page();
tid_t create_new_thread(const char *file_name, char* fn_copy);
char* get_file_name(char* name, char* delim, char* pt);
void close_connection(tid_t tid);

#endif /* userprog/process.h */
