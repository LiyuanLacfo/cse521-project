#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void exit_sys(int);
void close_all(int);

int read_sys(int*);
bool create_sys(int *);
bool success;
bool remove_sys(int *);
unsigned tell_sys(int *);

void handle_halt(struct intr_frame *f, int* esp);

void handle_exit(struct intr_frame *f, int* esp);

void handle_wait(struct intr_frame *f, int* esp);

void handle_file_size(struct intr_frame *f, int* esp);

void handle_write(struct intr_frame *f, int* esp);

void handle_read(struct intr_frame *f, int* esp);

void handle_open(struct intr_frame *f, int* esp);

void handle_create(struct intr_frame *f, int* esp);

void handle_close(struct intr_frame *f, int* esp);

void handle_seek(struct intr_frame *f, int* esp);

void handle_exec(struct intr_frame *f, int* esp);

void handle_remove(struct intr_frame *f, int* esp);

void handle_tell(struct intr_frame *f, int* esp);

#endif /* userprog/syscall.h */
