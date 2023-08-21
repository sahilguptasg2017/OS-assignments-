#include <stdio.h>
#include <stdlib.h>
struct context
{
    int eip;
    int esp;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
};

enum proc_state{
    UNUSED,EMBRYO,SLEEPING,RUNNABLE,RUNNING,ZOMBIE
};
struct proc
{
    char *mem;
    uint sz;
    char *kstack;

    enum proc_state state;
    int pid;
    struct proc *parent;
    void *chan;
    int killed;
    struct file *ofile[NOFILE];
    struct inode *cwd;
    struct context context ;
    struct trapframe *tf;
};

