#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
// #include <semaphore.h>
#include "zemaphore.c"

int CHAIRS = 4;
int CUSTOMERS = 10;

zem_t customer_sem;
zem_t barber_sem;
zem_t mutex_sem;
int freeChair = 4;
int barberSleeping = 1;
int barberWorking = 1;
int customerArrived = 0;

void* barberFunc(void *arg) {
  int thread_id = *((int*)arg);
  while (1) {
    zem_down(&mutex_sem);

    if (freeChair == CHAIRS) {
      printf("barber %d is sleeping\n", thread_id);
      barberSleeping = 1;
      barberWorking = 0;
      if (customerArrived == CUSTOMERS) {
        zem_up(&mutex_sem);
        break;
      }
      zem_up(&mutex_sem);
      zem_down(&customer_sem);
      zem_down(&mutex_sem);
    }

    freeChair++;
    printf("barber %d is cutting hair\n", thread_id);
    barberWorking = 1;
    sleep(rand() % 10);
    printf("barber %d is done cutting hair\n", thread_id);
    barberWorking = 0;
    zem_up(&barber_sem);
    zem_up(&mutex_sem);
  }
}

void* customerFunc(void *arg) {
  sleep(rand() % 10);
  int thread_id = *((int*)arg);

  zem_down(&mutex_sem);
  printf("customer %d arrived\n", thread_id);
  customerArrived++;

  if (freeChair == 0) {
    printf("customer %d left\n", thread_id);
    zem_up(&mutex_sem);
    return NULL;
  }

  freeChair--;
  printf("customer %d is waiting\n", thread_id);

  if (barberSleeping == 1) {
    barberSleeping = 0;
    zem_up(&customer_sem);
  }
  printf("customer %d is getting hair cut\n", thread_id);

  while (barberWorking == 1) {
    zem_up(&mutex_sem);
    zem_down(&barber_sem);
    zem_down(&mutex_sem);
  }

  zem_up(&mutex_sem);
}

int main() {
  printf("Hello World\n");
  zem_init(&customer_sem, 0);
  zem_init(&barber_sem, 0);
  zem_init(&mutex_sem, 1);

  int *barber_thread_id;
  pthread_t *barberThread;
  int *customer_thread_id;
  pthread_t *customerThreads;

  barber_thread_id = (int *)malloc(sizeof(int));
  barberThread = (pthread_t *)malloc(sizeof(pthread_t));

  pthread_create(barberThread, NULL, barberFunc, (void *)barber_thread_id);

  customer_thread_id = (int *)malloc(sizeof(int) * CUSTOMERS);
  customerThreads = (pthread_t *)malloc(sizeof(pthread_t) * CUSTOMERS);

  for (int i = 0; i < CUSTOMERS; i++) {
    customer_thread_id[i] = i;
    pthread_create(&customerThreads[i], NULL, customerFunc, (void *)&customer_thread_id[i]);
  }

  for (int i = 0; i < CUSTOMERS; i++) {
    pthread_join(customerThreads[i], NULL);
  }
  pthread_join(*barberThread, NULL);

  return 0;
}