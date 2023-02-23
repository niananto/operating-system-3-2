#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

// Modify this program in such a way that the print
// statements of the threads are interleaved in the order 
// thread0, thread1, thread2, thread0, thread1, thread2,
// ... and so on

#define NUM_THREADS 3
#define NUM_ITER 10

zem_t zem[NUM_THREADS];
int turn = 0;

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for (int i = 0; i < NUM_ITER; i++) {

    zem_down(&zem[thread_id]);
    printf("This is thread %d\n", thread_id);
    turn = (turn + 1) % NUM_THREADS;
    zem_up(&zem[turn]);    
  }

  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    zem_init(&zem[i], 0);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    mythread_id[i] = i;
    pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
  }

  zem_up(&zem[0]);

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(mythreads[i], NULL);
  }

  return 0;
}
