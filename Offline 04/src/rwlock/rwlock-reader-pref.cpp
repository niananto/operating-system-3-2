#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
    rw->totalreaders=0;
    rw->writerlock=0;
    pthread_mutex_init(&rw->mutexvar,NULL);
    pthread_cond_init(&rw->readercond,NULL);
    pthread_cond_init(&rw->writercond,NULL);
 
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
    pthread_mutex_lock(&rw->mutexvar);
    while(rw->writerlock==1)
    {
      pthread_cond_wait(&rw->readercond,&rw->mutexvar);

    }
    rw->totalreaders=rw->totalreaders+1;
    pthread_mutex_unlock(&rw->mutexvar);

}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
    pthread_mutex_lock(&rw->mutexvar);
    rw->totalreaders=rw->totalreaders-1;
    if(rw->totalreaders==0)
    {
      pthread_cond_signal(&rw->writercond);
    }
    pthread_mutex_unlock(&rw->mutexvar);

}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
    pthread_mutex_lock(&rw->mutexvar);
    while(rw->writerlock==1 || rw->totalreaders>0)
    {
      pthread_cond_wait(&rw->writercond,&rw->mutexvar);
    }
    rw->writerlock=1;
    pthread_mutex_unlock(&rw->mutexvar);

}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
    pthread_mutex_lock(&rw->mutexvar);
    rw->writerlock=0;
    pthread_cond_broadcast(&rw->readercond);
    pthread_cond_signal(&rw->writercond);
    pthread_mutex_unlock(&rw->mutexvar);

}
