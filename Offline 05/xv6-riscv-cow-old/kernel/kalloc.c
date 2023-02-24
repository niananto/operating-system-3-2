// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

// KERNBASE == 0x80000000
static int reference_count[(PHYSTOP - KERNBASE) / PGSIZE];

static int idx_rc(uint64 pa) { // get reference count index
  return (pa - KERNBASE) / PGSIZE;
}

void add_rc(uint64 pa) { // add reference count
  int idx = idx_rc(pa);
  if (idx < 0 || idx >= (PHYSTOP - KERNBASE) / PGSIZE) {
    printf("add_rc: idx: %d, pa: %p\n", idx, pa);
    panic("add_rc: idx out of range");
  }
  reference_count[idx]++;
}

void sub_rc(uint64 pa) { // subtract reference count
  int idx = idx_rc(pa);
  if (idx < 0 || idx >= (PHYSTOP - KERNBASE) / PGSIZE) {
    printf("sub_rc: idx: %d, pa: %p\n", idx, pa);
    panic("sub_rc: idx out of range");
  }
  int rc = reference_count[idx];
  if (rc <= 0) {
    printf("sub_rc: idx: %d, pa: %p, rc: %d\n", idx, pa, rc);
    panic("sub_rc: rc <= 0; free page");
  }

  reference_count[idx] = rc - 1;
  
  rc = reference_count[idx];
  if (rc == 0) {
    kfree((void*)pa);
  }
}

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

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  memset(reference_count, 0, sizeof(reference_count));
  // printf("kinit: reference_count size: %d\n", sizeof(reference_count));
  // printf("kinit: reference_count[0]: %d\n", reference_count[0]);
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

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
  if(reference_count[idx_rc((uint64)pa)] > 1) { // >1 just sub 1
    sub_rc((uint64)pa);
    return;
  }
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
  reference_count[idx_rc((uint64)pa)] = 0;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  
  // initialize reference count to 1 when allocating pages
  if(r)
    add_rc((uint64)r);
    // reference_count[idx_rc((uint64)r)] = 1;

  return (void*)r;
}
