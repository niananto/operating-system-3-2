#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int produced_items, consumed_items, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;

void print_produced(int num, int master) {
  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {
  printf("Consumed %d by worker %d\n", num, worker);
}

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

// produce items and place in buffer
// modify code below to synchronize correctly
// producers must not try to produce when the buffer is full
// every item should be produced exactly once
// must only use pthreads condition variables for waiting and signaling
// busy waiting is not allowed

void do_fill(int value) {
  // the idea of iterating whole buffer is that
  // as curr_buf_size is less than max_buf_size
  // there must be one empty slot in the buffer
  // otherwise, this function would not be called
  // so iterating it once is enough
  // cause we'll always find at least one empty slot
  for (int i = 0; i < max_buf_size; i++) {
    if (buffer[i] == -1) {
      buffer[i] = value;
      curr_buf_size++;
      return;
    }
  }
}

int do_get() {
  // similar to do_fill, we'll always find at least one
  // non-empty slot in the buffer or this function would
  // not be called
  for (int i = 0; i < max_buf_size; i++) {
    if (buffer[i] != -1) {
      int ret = buffer[i];
      buffer[i] = -1;
      curr_buf_size--;
      return ret;
    }
  }
}

void *generate_requests_loop(void *data) {
  int thread_id = *((int *)data);

  while (1) {
    pthread_mutex_lock(&buffer_lock);

    if (produced_items == total_items) {
      pthread_cond_broadcast(&buffer_not_empty);
      pthread_mutex_unlock(&buffer_lock);
      return 0;
    }

    while (curr_buf_size == max_buf_size) {
      pthread_cond_wait(&buffer_not_full, &buffer_lock);

      if (produced_items == total_items) {
        pthread_cond_broadcast(&buffer_not_empty);
        pthread_mutex_unlock(&buffer_lock);
        return 0;
      }
    }

    do_fill(produced_items);
    print_produced(produced_items, thread_id);
    produced_items++;

    pthread_cond_signal(&buffer_not_empty);
    pthread_mutex_unlock(&buffer_lock);
  }

  return 0;
}

// consume items from buffer
void *consume_requests_loop(void *data) {
  int thread_id = *((int *)data);

  while (1) {
    pthread_mutex_lock(&buffer_lock);

    if (consumed_items == total_items) {
      pthread_cond_broadcast(&buffer_not_full);
      pthread_mutex_unlock(&buffer_lock);
      return 0;
    }

    while (curr_buf_size == 0) {
      pthread_cond_wait(&buffer_not_empty, &buffer_lock);

      if (consumed_items == total_items) {
        pthread_cond_broadcast(&buffer_not_full);
        pthread_mutex_unlock(&buffer_lock);
        return 0;
      }
    }

    int item = do_get();
    print_consumed(item, thread_id);
    consumed_items++;
    
    pthread_cond_signal(&buffer_not_full);
    pthread_mutex_unlock(&buffer_lock);
  }
  
  return 0;
}

// write function to be run by worker threads
// ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  int *worker_thread_id;
  pthread_t *worker_thread;
  produced_items = 0;
  consumed_items = 0;
  curr_buf_size = 0;

  int i;

  if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #num_masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }

  buffer = (int *)malloc(sizeof(int) * max_buf_size);
  for (int i = 0; i < max_buf_size; i++)
    buffer[i] = -1;

  // create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

  // create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);

  // wait for all threads to complete
  for (i = 0; i < num_masters; i++) {
    pthread_join(master_thread[i], NULL);
    printf("master %d joined\n", i);
  }

  for (i = 0; i < num_workers; i++) {
    pthread_join(worker_thread[i], NULL);
    printf("worker %d joined\n", i);
  }

  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);

  return 0;
}
