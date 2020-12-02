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

struct kmem{
  struct spinlock lock;
  struct run *freelist;
};

struct kmem kmems[3];

int getcpu(){
  push_off();
  int cpu=cpuid();
  pop_off();
  return cpu;
}

void printkmem(){
  printf("###########################################\n");
  for(int i=0;i<3;i++){
    int c=0;
    struct run *p=kmems[i].freelist;
    while(p!=0){
      p=p->next;
      c++;
    }
    printf("cpu id %d : %d blocks\n",i,c);
  }
  printf("###########################################\n");
  
}
void
kinit()
{

  printf("[kinit] cpu id %d\n",getcpu());
  for(int i=0;i<3;i++)
    initlock(&kmems[i].lock, "kmem");
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
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  int hart=getcpu();
  acquire(&kmems[hart].lock);
  r->next = kmems[hart].freelist;
  kmems[hart].freelist = r;
  release(&kmems[hart].lock);
}

void *
steal(){
  struct run * rs=0;
  for(int i=0;i<3;i++){
    acquire(&kmems[i].lock);
    if(kmems[i].freelist!=0){
      rs=kmems[i].freelist;
      kmems[i].freelist=rs->next;
      release(&kmems[i].lock);
      return (void *)rs;
    }
    release(&kmems[i].lock);
  }
  return (void *)rs;
}

void *
kalloc(void)
{
  struct run *r;
  int hart=getcpu();
  acquire(&kmems[hart].lock);
  r = kmems[hart].freelist;
  if(r)
    kmems[hart].freelist = r->next;
  release(&kmems[hart].lock);
  if(!r)
  {
    r=steal(hart);
  }
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}