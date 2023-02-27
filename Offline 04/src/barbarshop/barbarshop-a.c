#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>

int CHAIRS = 4;
int CUSTOMERS = 10;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t customer = PTHREAD_COND_INITIALIZER;
pthread_cond_t barber = PTHREAD_COND_INITIALIZER;
int freeChair = 4;
int barberSleeping = 1;
int barberWorking = 1;
int customerArrived = 0;

void* barberFunc(void *arg) {
  int thread_id = *((int*)arg);
  while (1) {
    pthread_mutex_lock(&mutex);
    if (freeChair == CHAIRS) {
      printf("barber %d is sleeping\n", thread_id);
      barberSleeping = 1;
      barberWorking = 0;
      if (customerArrived == CUSTOMERS) {
        pthread_mutex_unlock(&mutex);
        break;
      }
      pthread_cond_wait(&customer, &mutex);
    }

    // barbar is taking a customer
    freeChair++;
    printf("barber %d is cutting hair\n", thread_id);
    barberWorking = 1;
    sleep(rand() % 10);
    printf("barber %d is done cutting hair\n", thread_id);
    barberWorking = 0;
    pthread_cond_signal(&barber);
    pthread_mutex_unlock(&mutex);
  }
}

void* customerFunc(void *arg) {
  sleep(rand() % 10);
  int thread_id = *((int*)arg);

  pthread_mutex_lock(&mutex);
  printf("customer %d arrived\n", thread_id);
  customerArrived++;

  if (freeChair == 0) {
    printf("customer %d left\n", thread_id);
    pthread_mutex_unlock(&mutex);
    return NULL;
  }

  freeChair--;
  printf("customer %d is waiting\n", thread_id);

  if (barberSleeping) {
    barberSleeping = 0;
    pthread_cond_signal(&customer);
  }
  printf("customer %d is getting hair cut\n", thread_id);
  while (barberWorking) {
    pthread_cond_wait(&barber, &mutex);
  }
  pthread_mutex_unlock(&mutex);
}

int main() {
  printf("Hello World\n");

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