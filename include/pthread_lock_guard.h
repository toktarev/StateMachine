#pragma once
#include <errno.h>
#include <iostream>
#include <pthread.h>

class plock_guard final {
  pthread_mutex_t *mMutex{nullptr};

  bool mmIsLocked{false};

public:
  explicit plock_guard(pthread_mutex_t *theMutex) {
    mMutex = theMutex;
    int aResult = pthread_mutex_lock(theMutex);

    if (aResult != 0) {
      errno = aResult;
      perror("pthread_cond_wait");
    }

    mmIsLocked = true;
  }

  ~plock_guard() {
    if (mmIsLocked) {
      pthread_mutex_unlock(mMutex);
    }
  }
};