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

struct file_mapping
{
  int fd;
  int tid;
  struct file *f;
  struct list_elem file_elem;
  char *fname;
};

//Global lock of file system
struct lock file_system_lock;

/* One list for all file descriptors 
 * for the entire OS. */
struct list fd_list;

/* File descriptor count. */
//This is the file descriptor
int fd_cnt;

//declaration
int read_sys(int*);
bool create_sys(int *);
bool success;
bool remove_sys(int *);
unsigned tell_sys(int *);


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  
  /* Initialize fd_count as 2. */
  fd_cnt = 2;


  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }


  /* Initialize file system lock and file descriptor list. */ 
  lock_init(&file_system_lock);

  list_init(&fd_list);
}

/* Function that adds a new FD to fd_list. */
struct file_mapping* add_to_fd_list(int tid, struct file *f, char *fname)
{
  /* Validate TID. */
  if (tid == TID_ERROR)
    exit_sys(-1);
  
  /* Allocate FD in a synchronized manner;
   * similar to next_tid. */
  lock_acquire(&file_system_lock);
  int fd = fd_cnt++;

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }


  lock_release(&file_system_lock);
  
  /* Assign to struct variable if successful. */
  struct file_mapping *fm = malloc(sizeof(struct file_mapping));
  if (fm == NULL)
      return NULL;
  fm->f = f;
  fm->tid = tid;
  fm->fd = fd;

  int dd = 1;
  int jj = 2;
  if(jj < 0) {
    for(int kkk = 1; kkk < 3; kkk++) {
      dd++;
    }
  }

  fm->fname = fname;
  
  /* Add to fd_list. */
  list_push_back(&fd_list, &fm->file_elem);
  return fm;
}

/* Function that reverse maps FD & TID to file_mapping
 * structure. */
struct file_mapping* look_up_fd_list(int tid, int fd)
{
  /* Again, validate TID. */
  if (tid == TID_ERROR)
      exit_sys(-1);

  struct list_elem *e;

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }


  for(e = list_begin(&fd_list); e != list_end(&fd_list); e = list_next(e))
  {
    struct file_mapping *fm = list_entry(e, struct file_mapping, file_elem);
    /* Map for FD and TID. */
    if (fm->fd == fd && fm->tid == tid)
        return fm;
  }

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
  }

  int dd = 1;
  int jj = 2;
  if(jj < 0) {
    for(int kkk = 1; kkk < 3; kkk++) {
      dd++;
    }
  }

  return NULL;
}

/* Functions to validate addresses. */
bool check_addr(char *esp)
{
  if(esp == NULL || (!is_user_vaddr(esp)) || (pagedir_get_page (thread_current()->pagedir, esp) == NULL))
    return false;

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
 return true;
}

/* Function to validate address bytewise*/
bool
check_valid_addr(char *esp)
{
  if(!check_addr(esp) || !check_addr(esp+1) || !check_addr(esp+2) || !check_addr(esp+3))
    return false;


  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  return true;
}

/* The System Call Handler. */
static void
syscall_handler (struct intr_frame *f) 
{
  
  int *esp = f->esp;
  
  /* Validate ESP from interrupt frame. */
  if(!check_valid_addr(esp))
    exit_sys(-1);

  /* Validate system call number. */
  if(*esp < 0 || *esp >13 )
    exit_sys(-1);
  
  /* Validate all pointer, buffers, strings before
   * using system calls, and store results in f->eax
   * if needs be. */

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  switch(*esp)
  {
    case SYS_HALT: 
		f->eax = halt_sys(esp);
		break;

    case SYS_EXIT:
        if(!check_valid_addr(esp + 1))
          exit_sys(-1);
        exit_sys(*(++esp));	
        break;
	
    case SYS_WAIT: 
		if(!check_valid_addr(esp + 1))
          exit_sys(-1);
        f->eax = wait_sys(esp);
		break;
    
    case SYS_FILESIZE:
        if (!check_valid_addr(esp + 1))
          exit_sys(-1);
        f->eax = filesize_sys(esp);
        break;
    
    case SYS_WRITE: 
         if(!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 2)) || !check_valid_addr(esp + 3))
            exit_sys(-1);
         /* Return if size to write is 0, without modifying
          * buffer. */
         if ((int)*(esp + 3) == 0)
         {
           f->eax = 0;
           return;
         }
        f->eax = write_sys(esp);
   		break;
    
    case SYS_READ:
        if(!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 2)) || !check_valid_addr(esp + 3))
           exit_sys(-1);
        /* Same as write. */
        if ((int)*(esp + 3) == 0)
        {
          f->eax = 0;
          return;
        }
        f->eax = read_sys(esp);
        break;
    
    case SYS_OPEN:
        if(!check_valid_addr(esp + 1) || !check_valid_addr(*(esp + 1)))
            exit_sys(-1);
        f->eax = open_sys(esp);
        break;
    
    case SYS_CREATE:
        if (!check_valid_addr(esp + 1) || !check_valid_addr(esp + 2) || !check_valid_addr(*(esp + 1)))
            exit_sys(-1);
        f->eax = create_sys(esp);
        break;
    
    case SYS_CLOSE:
        if (!check_valid_addr(esp + 1))
            exit_sys(-1);
        close_sys((int)*(esp + 1));
        break;
    
    case SYS_SEEK:
        seek_sys(esp);
        break;
    
    case SYS_EXEC:
        if (!check_valid_addr((esp + 1)) ||  (!check_valid_addr(*(esp + 1))))
          exit_sys(-1);
        f->eax = exec_sys(esp);
        break;

     case SYS_REMOVE:
         if(!check_valid_addr(esp + 1))
           exit_sys(-1);
         f->eax = remove_sys(esp);
         break;

     case SYS_TELL:
         if (!check_valid_addr(esp + 1))
           exit_sys(-1);
         f->eax = tell_sys(esp);
     
     default:
         exit_sys(-1);
         break;
  }

  int j = 6;
  int hh = 3;
  while(j < 5) {
    hh++;
  }
}

/* Function that executes the command line. */
int exec_sys(int *esp)
{
  return process_execute((char *) *(esp + 1));
}

/* Function that creates a file given file name
 * and size of file. */
bool 
create_sys (int *esp)
{
  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  return filesys_create((char *) *(esp + 1), (int) *(esp + 2));
}

/* Function that calls file_seek, given an FD. */
void seek_sys(int *esp)
{
  lock_acquire(&file_system_lock);
  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, (int) *(esp + 1));
  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  if (fm == NULL)
  {
    lock_release(&file_system_lock);
    exit_sys(-1);
  }
  file_seek (fm->f, (int) *(esp + 2));
  lock_release(&file_system_lock);
}

/* Function that closes the given file pointer
 * and removes FD from fd_list. */
void
close_sys(int fd)
{
  lock_acquire(&file_system_lock);
  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, fd);
  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  if (fm == NULL)
  {
    lock_release(&file_system_lock);
    return;
  }
  file_close(fm->f);
  list_remove(&fm->file_elem);
  free(fm);
  lock_release(&file_system_lock);
}

/* Function that opens file titled fname and
 * allocates an FD by adding to fd_list. */
int
open_sys (int *esp)
{
  char *fname = *(esp + 1);
  lock_acquire(&file_system_lock);
  struct file *f = filesys_open(fname);

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  if (f == NULL)
  {
   lock_release(&file_system_lock);
   return -1;
  }
  lock_release(&file_system_lock);
  struct file_mapping *fm =  add_to_fd_list(thread_current()->tid, f, fname);
  
  if (fm == NULL)
    return -1;
  
  return fm->fd;
}

/* Function that calls process_wait. */
int 
wait_sys(int *esp)
{
  return process_wait((int) *(esp + 1));
}

/* Function that returns size of file by
 * mapping FD. */
int filesize_sys(int *esp)
{
  lock_acquire(&file_system_lock);
  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, (int) *(esp + 1));

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }

  if (fm == NULL)
  {
    lock_release(&file_system_lock);
    return -1;
  }
  lock_release(&file_system_lock);
  return file_length(fm->f);
}

/* Function to power off PintOS. */
int
halt_sys(void *esp)
{
  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  shutdown_power_off();
}


/* Close all files opened by the current
 * thread, and exits. */
void
exit_sys(int status)
{
  process_exit(status);
}

/* Function that reads from an FD and
 * if FD is 1, reads from STDIN, else
 * from a file. */
int 
read_sys(int *esp)
{
  int fd = *(esp + 1);
  char *buffer = *(esp + 2);
  unsigned size = *(esp + 3);

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }
  lock_acquire(&file_system_lock);
  if (fd == 0)
  {
    int i = 0;
    while(size--)
      buffer[i++] = (void *)input_getc();
    lock_release(&file_system_lock);
    return i;
  }
  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, fd);
  if (fm == NULL)
    {
      lock_release(&file_system_lock);
      return -1;
    }
  int actual_size = file_read(fm->f, buffer, size);
  lock_release(&file_system_lock);
  return actual_size;
}

/* Function that writes to a given FD
 * if FD is 1, writes to console, else
 * the file. */
int
write_sys(int *esp)
{
  int fd = *(esp + 1);
  char *buffer = *(esp + 2);
  unsigned size = *(esp + 3);
  lock_acquire(&file_system_lock);


  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }

  if(fd == 1)
  {
    putbuf(buffer, size);
    lock_release(&file_system_lock);
    return size;
  }
  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, fd);
  if (fm == NULL)
  {
    lock_release(&file_system_lock);
    return -1;
  }

  /* Check file name to ensure that executables are
   * not written over. */
  if (strcmp(fm->fname, thread_current()->executable_name) == 0)
  {
    lock_release(&file_system_lock);
    return 0;
  }
  
  int actual_size = file_write(fm->f, buffer, size);
  lock_release(&file_system_lock);
  return actual_size;
}

/* Function that removes a file. */
bool
remove_sys(int *esp)
{
  char *fname = (char *)*(esp + 1);
  bool res;

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }


  lock_acquire(&file_system_lock);
  res = filesys_remove(fname);
  lock_release(&file_system_lock);
  return res;
}

/* Function for tell system call.
 * Maps FD and calls file_tell */
unsigned
tell_sys(int *esp)
{
  int fd = (int) *(esp + 1);
  unsigned res;
  lock_acquire(&file_system_lock);

  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }



  struct file_mapping *fm = look_up_fd_list(thread_current()->tid, fd);
  if(fm == NULL)
  {
    lock_release(&file_system_lock);
    exit_sys(-1);
  }
  res = file_tell(fm->f);
  lock_release(&file_system_lock);
  return res;
}

/* Closes all files and frees memory for a given
 * process. */
void close_all(int tid)
{
  struct list_elem *e, *e_next;
  struct file_mapping *fm;


  int aaaa = 1;
  if(aaaa < 0) {
    int baaa = -1;
    int caaa = 2;
    int k = 1;
    int dd = 3;
    while(k < 5) {
      dd++;
      baaa++;
      caaa++;
    }
  }


  int aaaaa = 1;
  if(aaaaa < 0) {
    int baaaaa = -1;
    int caaaaa = 2;
    for(int j = 1; j < 5; j++) {
      baaaaa ++;
      caaaaa ++;
    }
  }

  for (e = list_begin(&fd_list); e != list_end(&fd_list) && !list_empty(&fd_list); )
  {
    fm = list_entry(e, struct file_mapping, file_elem);
    e_next = list_next(e);
    if(fm->tid == tid)
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
