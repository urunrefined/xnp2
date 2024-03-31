#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Runable {
  public:
    virtual void run() = 0;
    virtual void finish() = 0;
    ~Runable();
};

void *run(void *);

class Thread {
    Thread(Thread &) = delete;
    Runable &runable;

    pthread_t thread;

  public:
    Thread(Runable &runable_);
    ~Thread();
};

class Mutex {
  public:
    Mutex(Mutex &) = delete;

    pthread_mutex_t mutex;

    Mutex();
    ~Mutex();
};

class LockGuard {
    Mutex &mutex;

  public:
    LockGuard(LockGuard &) = delete;

    LockGuard(Mutex &mutex_);
    ~LockGuard();
};

#endif // THREAD_H
