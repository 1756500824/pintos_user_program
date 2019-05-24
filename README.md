# Pintos User Program

![progress](http://progressed.io/bar/100?title=done) ![ubuntu-version](https://img.shields.io/badge/ubuntu-18.04-yellow.svg) ![ubuntu-version](https://img.shields.io/badge/ubuntu-16.04-green.svg) 

### Group member

[![author1](https://img.shields.io/badge/Author-Qiyang%20He-orange.svg)](https://github.com/1756500824)
[![author2](https://img.shields.io/badge/Author-Yue%20Gong-blue.svg)](https://github.com/snowgy)

### Task 1: Argument passing

##### Data structure

No new data structure is needed.

##### Algorithm 

Use `strtok_r` to split the input argument string, and got each argument. Then push them to the process stack by the order according to the stack structure below

<img src="http://ww1.sinaimg.cn/large/74c2bf2dgy1g37mm9rcs3j20o40f40vf.jpg" width="500px"/>

##### Synchronization

No synchronization operation needed.

##### Rationale

We use a smart method to implement word-align (bit operation => ptr&0xfffffffc).

### Task 2: Process Control Syscalls

##### Data structure

Add a children list in `struct thread`

Add a struct to record the information of a child process

```c
struct child_process
  {
    tid_t tid;                            /* Thread identifier. */
    struct list_elem childelem;           /* List element for children list. */
    bool exited;                          /* Whether it has exited*/
    bool waited;                          /* Whether it has waited for some child. */
    struct semaphore sema;                /* Wait semaphore. */
    int exit_status;                      /* Status when it exits. */
  };
```

##### Algorithm

The syscall stack is as below

```
f->esp   -----------------------------
	|           SYS_CODE          |
 	 -----------------------------
	|  arg[0] ptr -OR- int_value  |
 	 -----------------------------
	|  arg[1] ptr -OR- int_value  |
	 -----------------------------
	|  arg[2] ptr -OR- int_value  |
	 -----------------------------
```

We will first get the `SYS_CODE` to decide which syscall is used. Then we will read the arguments and do validation. The major challange here is to valify whether the argument pointer is valid. We should not only check whether the address of the pointer is a valid user address, but also check whether it is mapped in the kenerl address.

The most difficult part is to implement `exec` and `wait`. When `exec`, the parent process must wait until the child process finished `load` operation. `wait` is to wait for a child process to finish, then the parent process can proceed executing.

##### Synchronization

When `exec`, the parent process must wait until the child process finished load operation. `wait` is to wait for a child process to finish, then the parent process can proceed executing.We use semaphore to implement the synchronization.

### Task 3: File Operation Syscalls

##### Data Stucture

In the `struct thread`, add a file list which records files the process opens.

```c
struct file_control_block
{
  struct file *process_file;
  int fd;
  struct list_elem file_elem;
};
```

##### Algorithm

The implementaion of file operation is straightforward because the basic implementation is given in the `filesys/file.c`. The only point we need to pay attention to is the synchronization. We set a global lock named `filesys_lock`. Every time a process wants to user file operation syscall, it must firstly acquire the `filesys_lock`.

##### Synchronization

The only point we need to pay attention to is the synchronization. We set a global lock named `filesys_lock`. Every time a process wants to user file operation syscall, it must firstly acquire the `filesys_lock`.

### Questions

#### What exactly did each member do? What went well, and what could be improved? 

Qiyang he is mainly responsible for task1 and task2

Yue Gong is mainly responsible for task3.

We corporate well, finished each function step by step. We could furthur refactor our code to make it more readable. 

#### Does your code exhibit any major memory safety problems (especially regarding C strings), memory leaks, poor error handling, or race conditions? 

No. We will free every page that was allocated before. And we use semaphore and lock to solve race conditions.

#### Did you use consistent code style? Your code should blend in with the existing Pintos code. Check your use of indentation, your spacing, and your naming conventions. 

Yes. We follow the pintos coding style.

#### Is your code simple and easy to understand? 

Yes. We had no redundant code and each line of our codes has clear puporse.

#### If you have very complex sections of code in your solution, did you add enough comments to explain them? 

Yes. For complex section, we always split it into small and simple sections. For nearly every code block, we have clear comment. You can easily get the idea of our implementation.

#### Did you leave commented-out code in your final submission? 

Yes. Our final submission has clear and detailed comments.

#### Did you copy-paste code instead of creating reusable functions? 

We encapsulate every common procedure into reusable functions.

#### Are your lines of source code excessively long? (more than 100 characters) 

No.

#### Did you re-implement linked list algorithms instead of using the provided list manipulation

No. We use the original linked list algorithm.

### Result

#### Platform: `ubuntu 18.04 & ubuntu 16.04`

**We pass all 80 tests**

<img src="https://ws1.sinaimg.cn/mw690/74c2bf2dgy1g37mznu0knj209j0nomx6.jpg" width="300px"/>
