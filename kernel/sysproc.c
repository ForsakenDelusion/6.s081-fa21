#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_mmap(void)
{
  uint64 size; // 映射过去的字节数，可能与文件大小不一样
  int prot; // 指示内存是否应映射为可读、可写，以及/或者可执行的
  int flags; // flags要么是MAP_SHARED（映射内存的修改应写回文件），要么是MAP_PRIVATE（映射内存的修改不应写回文件）
  int fd; // 映射的文件的打开文件描述符

  if(argaddr(1, &size) != 0)
  {
    return -1;
  }

  if(argint(2, &prot) != 0) 
  {
    return -1;
  }

  if(argint(3, &flags) != 0)
  {
    return -1;
  }

 if(argint(4, &fd) != 0)
  {
    return -1;
  }

  struct proc* p = myproc();
  struct vma* v = p->vma;

  struct file* file;
  file = p->ofile[fd];

  if(!file->writable && (prot & PROT_WRITE) && (flags & MAP_SHARED))
  return -1;
  

  
  return 0; 





  return 1;
}

uint64
sys_munmap(void)
{
  return 1;
}
