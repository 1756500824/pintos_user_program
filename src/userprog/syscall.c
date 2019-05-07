#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/kernel/stdio.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "process.h"
#ifndef ARGLEN
#define ARGLEN 5
#endif

typedef int pid_t;

static void syscall_handler (struct intr_frame *);
static void is_valid_ptr (const void *ptr);
void halt (void) NO_RETURN;
void exit (int status) NO_RETURN;
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

struct lock filesys_lock;


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init (&filesys_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // printf ("system call!\n");
  // thread_exit ();
  //first check if f->esp is a valid pointer)
  is_valid_ptr (f->esp);
  //cast f->esp into an int*, then dereference it for the SYS_CODE

  int *p = f->esp;
  int argv[ARGLEN];

  switch(*(int*)f->esp)
  {
    case SYS_HALT:
    {
      halt ();
      //Implement syscall HALT
      break;
    }
    case SYS_EXIT:
    {
      //Implement syscall EXIT
      read_args (1, argv, f);
      exit (argv[0]);
      break;
    }
    case SYS_EXEC:
    {
      read_args (1, argv, f);
      f->eax = exec (argv[0]);
      break;
    }
    case SYS_WAIT:
    {
      read_args (1, argv, f);
      //printf("---READ: %d---\n", argv[0]);
      //printf("---SYS: %d---\n", *(p + 1));
      f->eax = wait (*(p + 1));
      break;
    }
    case SYS_CREATE:
    {
      read_args (2, argv, f);
      create (argv[0], argv[1]);
      break;
    }
    case SYS_REMOVE:
    {
      read_args (1, argv, f);
      remove (argv[0]);
      break;
    }
    case SYS_OPEN:
    {
      read_args (1, argv, f);
      break;
    }    
    case SYS_FILESIZE:
    {
      read_args (1, argv, f);
      break;
    }
    case SYS_READ:
    {
      read_args (3, argv, f);
      break;
    }
    case SYS_WRITE:
    {
      read_args (3, argv, f);
      // int fd = *((int*)f->esp + 1);
      // void* buffer = (void*)(*((int*)f->esp + 2));
      // unsigned size = *((unsigned*)f->esp + 3);
      f->eax = write (argv[0], argv[1], argv[2]);
      break;
    } 
    case SYS_SEEK:
    {
      read_args (2, argv, f);
      break;
    }
    case SYS_TELL:
    {
      read_args (1, argv, f);
      break;
    }
    case SYS_CLOSE:
    {
      read_args (1, argv, f);
      break;
    }
  }
}

void 
read_args (int n, int *argv, struct intr_frame *f)
{
  int i;
  int *ptr;
  /*if (*(int*)f->esp == SYS_WAIT) {
    hex_dump((uintptr_t)f->esp, f->esp, sizeof(char)*16, true);
  }*/
  for (i = 0; i < n; ++i) 
  {
    ptr = ((int *)f->esp + 1 + i);
    is_valid_ptr ((void *)ptr);
    argv[i] = *ptr;
    /*if (*(int*)f->esp == SYS_WAIT)
      printf ("argv[%d]---%d---\n", i, argv[i]);*/
  }
}

static void 
is_valid_ptr (const void *ptr)
{
  if (ptr == NULL || !is_user_vaddr (ptr) || !is_user_vaddr_above (ptr))
  {
    exit (-1);
  } 
}

void 
halt ()
{
  shutdown_power_off ();
}

void 
exit (int status)
{
  printf ("%s: exit(%d)\n", thread_current ()->name, status);
  thread_exit ();
}

pid_t 
exec (const char *file)
{
  //printf ("---exe id: %d---\n", thread_current ()->tid);
  pid_t pid = -1;
  lock_acquire (&filesys_lock);
  pid = process_execute (file);
  lock_release (&filesys_lock);
  //printf ("---%d---\n", pid);
  return pid;
}

int 
wait (pid_t child_tid)
{
  //printf ("---wait id: %d---\n", thread_current ()->tid);
  int res = 0;
  //printf("---start wait---\n");
  res = process_wait (child_tid);
  //printf("---%d---\n", child_tid);
  //printf("---end wait %d---\n", res);
  return res;
}

bool 
create (const char *file, unsigned initial_size)
{
  bool flag;
  lock_acquire (&filesys_lock);
  flag = filesys_create (file, initial_size);
  //printf ("---1 Done---\n");
  lock_release (&filesys_lock);
  //printf ("---2 Done---\n");
  return flag;
}

bool 
remove (const char *file)
{
  bool flag;
  lock_acquire (&filesys_lock);
  flag = filesys_remove (file);
  lock_release (&filesys_lock);
  return flag;
}

int 
open (const char *file);

int 
filesize (int fd);

int 
read (int fd, void *buffer, unsigned length);

int 
write (int fd, const void* buffer, unsigned size)
{
  if (fd == STDOUT_FILENO)
  {
    putbuf (buffer, size);
    return;
  }
}

void 
seek (int fd, unsigned position);

unsigned 
tell (int fd);

void 
close (int fd);


