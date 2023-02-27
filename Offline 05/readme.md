Following [this](https://blog.csdn.net/qq_53111905/article/details/121000620)

## Copy on Write (CoW)

When we create a child process, instead of creating, allocating and copying the contents to new physical memory, we can actually share the physical memory page of the parent process directly. So here, we can set the PTE of the child process to point to the physical memory page corresponding to the parent process.

Once the child process wants to modify the contents of these memories, the corresponding updates should be invisible to the parent process, because we want strong isolation between parent and child processes. In order to ensure the isolation between processes, we can set the PTE flags of the parent process and the child process here to be read-only.

At some point, when we need to change the content of memory, we get page fault.

After getting the page fault, we need to copy the corresponding physical page. Assuming that the child process is executing the store instruction now, then we will allocate a new physical memory page, then copy the physical memory page related to the page fault to the newly allocated physical memory page, and map the newly allocated physical memory page to child process. At this time, the newly allocated physical memory page is only visible to the address space of the child process, so we can set the corresponding PTE to be readable and writable, and we can re-execute the store instruction. In fact, for the physical page that triggered the page fault just now, because it is only visible to the parent process now, the corresponding PTE becomes readable and writable for the parent process.

But for the physical memory page here, there are multiple user processes or multiple address spaces pointing to the same physical memory page. For example, we need to be more careful when the parent process exits, because we have to judge whether it can Immediately release the corresponding physical page. If there are child processes still using these physical pages, and the kernel releases these physical pages, we will have problems. So what is the basis for releasing the memory page now?

We need to count the references of each physical memory page. When we release the virtual page, we decrement the reference number of the physical memory page by 1. If the reference number is equal to 0, then we can release the physical memory page. So in copy-on-write lab, you need to introduce some additional data structures or metadata information to complete reference counting.

### Course of action

The modification uvmcopy()maps the physical pages of the parent process to the child process instead of allocating new pages. Clear flags in PTEs of child and parent processes PTE_W.

Modified usertrap()to identify page faults . When a COW page has a page fault, kalloc()allocate a new page using , and copy the old page to the new page, then add the new page to the PTE and set it PTE_W.

Ensures that each physical page is freed when the last PTE's reference to it is dropped - not before. A good way to do this is to keep for each physical page a "reference count" of the number of user page tables that reference that page. When kalloc()a page is allocated, the page's reference count is set to 1. Increment the page's reference count when forkcausing child processes to share the page; decrement the page's reference count whenever any process removes the page from its page table. kfree()Pages should only be put back on the free list when the reference count reaches zero. These counts can be stored in a fixed-size integer array. You have to have a plan for how to index the array and how to choose the size of the array. kinit()For example, you could index the array by the physical address of the page divided by 4096 and give the array the number of elements equal to the highest physical address of all pages placed in the free list in ***kalloc.c*** .

Modified copyout()to use the same scheme as page faults when COW pages are encountered.

hint:

The lazy page allocation experiments may have familiarized you with a lot of copy-on-write-related xv6 kernel code. However, you should not base this lab on your lazy allocation solution; instead, follow the instructions above to start with a fresh xv6.

There is a potentially useful way to record whether each PTE is a COW mapping. You can use the RSW (reserved for software) bit in the RISC-V PTE to achieve this.

usertestsCheck cowtestthe scenarios that don't test, so don't forget that both tests need to pass completely.

At the end of ***kernel/riscv.h*** there are some useful macros and definitions of page table flags.

If there is a COW page fault and no memory is available, the process should be terminated.
