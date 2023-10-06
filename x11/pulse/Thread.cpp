#include "Thread.h"

Thread::Thread(Runable &runable_) : runable(runable_) {
    pthread_create(&thread, 0, run, &runable);
}

Thread::~Thread() {
    runable.finish();
    pthread_join(thread, 0);
}

Runable::~Runable() {}

void *run(void *runable) {
    try {
        ((Runable *)runable)->run();
    } catch (...) {
    }

    return 0;
}

Mutex::Mutex() { pthread_mutex_init(&mutex, 0); }

Mutex::~Mutex() { pthread_mutex_destroy(&mutex); }

LockGuard::LockGuard(Mutex &mutex_) : mutex(mutex_) {
    pthread_mutex_lock(&mutex.mutex);
}

LockGuard::~LockGuard() { pthread_mutex_unlock(&mutex.mutex); }
