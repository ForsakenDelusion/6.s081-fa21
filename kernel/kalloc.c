// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};


struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct cpufreearr {
  struct spinlock cpulock;
  struct run* cpufreelist;
};

struct cpufreearr freelistarr[NCPU];

int
mycpuid()
{
  push_off();
  int id = cpuid();
  pop_off();
  return id;
}

void
kinit()
{ 
  initlock(&kmem.lock, "kmem");
  for (int i = 0; i < NCPU; i++) {
    initlock(&freelistarr[i].cpulock,"kmem-cpulock");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *cr;
  int id = mycpuid();

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  cr = (struct run*)pa;
  acquire(&freelistarr[id].cpulock);
  cr->next = freelistarr[id].cpufreelist;
  freelistarr[id].cpufreelist = cr;
  release(&freelistarr[id].cpulock);


}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *cr;
  struct run* rr;

  int id = mycpuid();
  acquire(&freelistarr[id].cpulock);

  cr = freelistarr[id].cpufreelist;

  if (!cr) { // 当前cpu维护的freelist没有空闲内存了，要偷一点
  int stealpage = 64;
  
  for (int i = 0;i < NCPU; i++) {

    if (i == id) continue;
    acquire(&freelistarr[i].cpulock);
    rr = freelistarr[i].cpufreelist;
    while (rr && stealpage) {
    freelistarr[i].cpufreelist = rr->next;
    rr->next = cr;
    cr = rr;
    rr = freelistarr[i].cpufreelist;
    stealpage--;
    }
    release(&freelistarr[i].cpulock);
    if (stealpage == 0) break;
  }
  }
  if(cr)
    freelistarr[id].cpufreelist = cr->next;
  release(&freelistarr[id].cpulock);

  if(cr)
    memset((char*)cr, 5, PGSIZE); // fill with junk
  return (void*)cr;
  
}
