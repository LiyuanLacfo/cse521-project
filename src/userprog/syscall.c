#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include <string.h>
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include <stdlib.h>

//This is the file structure

struct file_description
{
  int fd; //file descriptor
  int tid; //the thread id
  struct file *f; //file address
  char *fname; //file name
  struct list_elem file_elem; //element for find in the loop
};

struct lock file_system_lock;

struct list fd_list;

/* File descriptor count. */
//This is the file descriptor
int fd_cnt;

//declaration
//bool success;


int read_sys(int*);
bool create_sys(int *);
bool remove_sys(int *);
unsigned tell_sys(int *);

void delete_fd(struct file_description *fk);

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

static void syscall_handler (struct intr_frame *);

void filesys_acquire (void);

void filesys_release (void);

void fill_file_desc(struct file *f, int tid, int fd, char *fname, struct file_description *fm);
struct file_description* create_fm(struct file *f, int tid, int fd, char *fname);

void init() {
    fd_cnt = 2;
    /* Initialize file system lock and file descriptor list. */
    lock_init(&file_system_lock);
    list_init(&fd_list);
}

void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
    init();
}

int make_fd() {
    filesys_acquire();
    int fd = fd_cnt++;
    filesys_release();
    return fd;
}

struct file_description* create_fm(struct file *f, int tid, int fd, char *fname) {
    struct file_description *fm = malloc(sizeof(struct file_description));
    if (fm == NULL) return NULL;
    fill_file_desc(f, tid, fd, fname, fm);
    list_push_back(&fd_list, &fm->file_elem);
    return fm;
}
struct file_description* fill_fd_list(int tid, struct file *f, char *fname)
{

        if (tid == TID_ERROR) exit_sys(-1);
        int fd = make_fd();
        return create_fm(f, tid, fd, fname);
}

void fill_file_desc(struct file *f, int tid, int fd, char *fname, struct file_description *fm) {
    fm->f = f;
    fm->tid = tid;
    fm->fd = fd;
    fm->fname = fname;
}

struct file_description* seek_fd_list(int tid, int fd)
{
  if (tid == TID_ERROR) exit_sys(-1);
  struct list_elem *e;

  e = list_begin(&fd_list);
  while(e != list_end(&fd_list)) {
      struct file_description *fm = list_entry(e, struct file_description, file_elem);
      if (fm->fd == fd && fm->tid == tid)
          return fm;
      e = list_next(e);
  }
  return NULL;
}


bool check_addr(char *esp)
{
    bool flag = (esp == NULL) || (!is_user_vaddr(esp)) || (pagedir_get_page (thread_current()->pagedir, esp) == NULL);
    if(flag) return false;
    return true;
}


bool
check_valid_addr(char *esp)
{
    bool flag = !check_addr(esp) || !check_addr(esp+1) || !check_addr(esp+2) || !check_addr(esp+3);
    if(flag) return false;

    return true;
}



void handle_halt(struct intr_frame *f, int* esp) {
    shutdown_power_off();
}

void handle_exit(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1)) exit_sys(-1);
    process_exit(*(++esp));
}

void handle_wait(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1)) exit_sys(-1);
    f->eax = process_wait((int) *(esp + 1));
}

void handle_file_size(struct intr_frame *f, int* esp) {
    if (!check_valid_addr(esp + 1)) exit_sys(-1);
    f->eax = filesize_sys(esp);
}

void handle_write(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 2)) || !check_valid_addr(esp + 3))
        exit_sys(-1);

    bool flag = (int)*(esp + 3) == 0;
    if (flag)
    {
        f->eax = 0;
        return;
    }
    f->eax = write_sys(esp);
}

void handle_read(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 2)) || !check_valid_addr(esp + 3))
        exit_sys(-1);
    bool flag = (int)*(esp + 3) == 0;
    if (flag)
    {
        f->eax = 0;
        return;
    }
    f->eax = read_sys(esp);
}

void handle_open(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1) || !check_valid_addr(*(esp + 1)))
        exit_sys(-1);
    f->eax = open_sys(esp);
}

void handle_create(struct intr_frame *f, int* esp) {
    if (!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 1)))
        exit_sys(-1);
    f->eax = create_sys(esp);
}

void handle_close(struct intr_frame *f, int* esp) {
    if (!check_valid_addr(esp + 1)) exit_sys(-1);
    close_sys((int)*(esp + 1));
}

void handle_seek(struct intr_frame *f, int* esp) {
    seek_sys(esp);
}

void handle_exec(struct intr_frame *f, int* esp) {
    if (!check_valid_addr((esp + 1)) ||  (!check_valid_addr(*(esp + 1))))
        exit_sys(-1);
    f->eax = exec_sys(esp);
}

void handle_remove(struct intr_frame *f, int* esp) {
    if(!check_valid_addr(esp + 1)) exit_sys(-1);
    f->eax = remove_sys(esp);
}

void handle_tell(struct intr_frame *f, int* esp) {
    if (!check_valid_addr(esp + 1)) exit_sys(-1);
    f->eax = tell_sys(esp);
}

int exec_sys(int *esp)
{
    return process_execute((char *) *(esp + 1));
}

bool
create_sys (int *esp)
{
    return filesys_create((char *) *(esp + 1), (int) *(esp + 2));
}


void seek_sys(int *esp)
{
    filesys_acquire();
    struct file_description *fm = seek_fd_list(thread_current()->tid, (int) *(esp + 1));
    bool flag = fm == NULL;
    if (flag)
    {
        filesys_release();
        process_exit(-1);
    }
    file_seek (fm->f, (int) *(esp + 2));
    filesys_release();
}

void delete_fd(struct file_description *fk) {
    file_close(fk->f);
    list_remove(&fk->file_elem);
    free(fk);
}
void
close_sys(int fd)
{
    filesys_acquire();
    struct file_description *fm = seek_fd_list(thread_current()->tid, fd);

    //modify
    struct file_description *fk = fm;

    bool flag = fk == NULL;
    if (flag)
    {
        filesys_release();
        return;
    }
    delete_fd(fk);
    filesys_release();
}

int get_fd(struct file* ff, char* fname) {
    struct file_description *fm =  fill_fd_list(thread_current()->tid, ff, fname);
    bool flag = fm == NULL;
    if (flag) return -1;
    return fm->fd;
}

int
open_sys (int *esp)
{
    char *fname = *(esp + 1);
    filesys_acquire();
    struct file *f = filesys_open(fname);
    struct file *ff = f;

    bool flag = ff == NULL;

    if (flag)
    {
        filesys_release();
        return -1;
    }
    filesys_release();
    return get_fd(ff, fname);
}


//int
//wait_sys(int *esp)
//{
//    return process_wait((int) *(esp + 1));
//}


int filesize_sys(int *esp)
{
    filesys_acquire();
    struct file_description *fm = seek_fd_list(thread_current()->tid, (int) *(esp + 1));
    struct file_description *fk = fm;

    bool flag = fk == NULL;

    if (flag)
    {
        filesys_release();
        return -1;
    } else {
        filesys_release();
    }
    return file_length(fk->f);
}

void
exit_sys(int status)
{
    process_exit(status);
}

int get_size(int fd, char *buff, unsigned sizes) {
    struct file_description *fm = seek_fd_list(thread_current()->tid, fd);
    struct file_description *fk = fm;
    if (fk == NULL)
    {
        filesys_release();
        return -1;
    }
    int res = file_read(fk->f, buff, sizes);
    filesys_release();
    return res;
}

int
read_sys(int *esp)
{
    int fd = *(esp + 1);
    char *buffer = *(esp + 2);
    unsigned size = *(esp + 3);

    int fdd = fd;
    char *buff = buffer;
    unsigned  sizes = size;

    filesys_acquire();
    if (fdd == 0)
    {
        filesys_release();
        return (int)input_getc();
    }
//    struct file_description *fm = seek_fd_list(thread_current()->tid, fd);
//    struct file_description *fk = fm;
//    if (fk == NULL)
//    {
//        filesys_release();
//        return -1;
//    }
//    int actual_size = file_read(fk->f, buff, sizes);
//    filesys_release();
//    return actual_size;
    return get_size(fd, buff, sizes);
}


int get_write_size(int fd, char* buff, unsigned sizes) {
    struct file_description *fm = seek_fd_list(thread_current()->tid, fd);
    struct file_description *fk = fm;
    bool flag = (fk == NULL);
    if (flag)
    {
        filesys_release();
        return -1;
    }

    flag = (strcmp(fm->fname, thread_current()->executable_name) == 0);
    if (flag)
    {
        filesys_release();
        return 0;
    } else {
        int res = file_write(fk->f, buff, sizes);
        filesys_release();
        return res;
    }
}


int
write_sys(int *esp)
{
    int fd = *(esp + 1);
    char *buffer = *(esp + 2);
    unsigned size = *(esp + 3);

    int fdd = fd;
    char* buff = buffer;
    unsigned  sizes = size;

    lock_acquire(&file_system_lock);

    bool flag = (fdd == 1);
    if(flag)
    {
        putbuf(buff, sizes);
        lock_release(&file_system_lock);
        return sizes;
    }
    return get_write_size(fd, buff, sizes);
}

bool remove_res(char* fname) {
    bool res;
    filesys_acquire();
    res = filesys_remove(fname);
    filesys_release();
    return res;
}

bool
remove_sys(int *esp)
{
    char *fname = (char *)*(esp + 1);
//    bool res;
//    filesys_acquire();
//    res = filesys_remove(fname);
//    filesys_release();
//    return res;
    return remove_res(fname);
}


unsigned
tell_sys(int *esp)
{
    int fd = (int) *(esp + 1);
    unsigned offset;
    lock_acquire(&file_system_lock);


    struct file_description *fm = seek_fd_list(thread_current()->tid, fd);
    bool flag = (fm == NULL);
    if(flag)
    {
        lock_release(&file_system_lock);
        exit_sys(-1);
    }
    offset = file_tell(fm->f);
    lock_release(&file_system_lock);
    return offset;
}

static void
syscall_handler (struct intr_frame *f) 
{
  
  int *esp = f->esp;
  bool flag = !check_valid_addr(esp);
  if(flag) exit_sys(-1);
  flag = *esp < 0 || *esp >13;
  if(flag) exit_sys(-1);

  if(*esp == SYS_HALT) {
    handle_halt(f, esp);
  } else if(*esp == SYS_EXIT) {
    handle_exit(f, esp);
  } else if(*esp == SYS_WAIT) {
    handle_wait(f, esp);
  } else if(*esp == SYS_FILESIZE) {
    handle_file_size(f, esp);
  } else if(*esp == SYS_WRITE) {
    handle_write(f, esp);
  } else if(*esp == SYS_READ) {
      handle_read(f, esp);
  } else if(*esp == SYS_OPEN) {
      handle_open(f, esp);
  } else if(*esp == SYS_CREATE) {
        handle_create(f, esp);
  } else if(*esp == SYS_CLOSE) {
        handle_close(f, esp);
  } else if(*esp == SYS_SEEK) {
        handle_seek(f, esp);
  } else if(*esp == SYS_EXEC) {
        handle_exec(f, esp);
  } else if(*esp == SYS_REMOVE) {
        handle_remove(f, esp);
  } else if(*esp == SYS_TELL) {
        handle_tell(f, esp);
  } else {
      exit_sys(-1);
  }
}


/* Closes all files and frees memory for a given
 * process. */
void close_all(int tid)
{
  struct list_elem *e, *e_next;
  struct file_description *fm;

  e = list_begin(&fd_list);
  while(e != list_end(&fd_list) && !list_empty(&fd_list)) {
      fm = list_entry(e, struct file_description, file_elem);
      e_next = list_next(e);
      bool flag = fm->tid == tid;
      if(flag)
      {
          lock_acquire(&file_system_lock);
          file_close(fm->f);
          list_remove(e);
          free(fm);
          lock_release(&file_system_lock);
          if (list_empty(&fd_list))
              return;
      }
      e = e_next;
  }
}

//static struct file *
//thread_fd_get (int fd)
//{
//    struct thread *curr = thread_current ();
//    struct thread_fd *tfd;
//    struct list_elem *e;
//
//    if (fd < 2 || fd >= curr->max_fd)
//        return NULL;
//    for (e = list_begin (&curr->fd_list); e != list_end (&curr->fd_list);
//         e = list_next (e))
//    {
//        tfd = list_entry (e, struct thread_fd, elem);
//        if (tfd->fd == fd)
//            return tfd->file;
//    }
//    return NULL;
//}
//
///* Add FILE to fd_list of current thread and return its fd. */
//static int
//thread_fd_insert (struct file *file)
//{
//    struct thread *curr = thread_current ();
//    struct thread_fd *tfd = (struct thread_fd *)
//            malloc (sizeof (struct thread_fd));
//
//    tfd->fd = curr->max_fd++;
//    tfd->file = file;
//    list_push_back (&curr->fd_list, &tfd->elem);
//    return tfd->fd;
//}

/* Acquire the filesys_lock to usage file system. */
void
filesys_acquire (void)
{
    lock_acquire (&file_system_lock);
}

/* Release the filesys_lock. */
void
filesys_release (void)
{
    lock_release (&file_system_lock);
}
