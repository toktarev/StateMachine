
#pragma once

#include "StateMachineResource.h"
#include "shm.h"
#include <cstddef>
#include <fstream>
#include <memory>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <utility>

class StateMachineManager {

protected:
  pthread_condattr_t mCondAttr{};
  pthread_mutexattr_t mMutexAttr{};

  std::unique_ptr<StateMachineResource<char>> mBufferResource{};

  std::unique_ptr<StateMachineResource<shm_sync>> mSyncResource{};

  void initMutexAttr() {
    if (pthread_mutexattr_init(&mMutexAttr) < 0) {
      perror("Can't pthread_mutexattr_init");
      throw std::runtime_error(std::to_string(errno));
    }

    if (pthread_mutexattr_setpshared(&mMutexAttr, PTHREAD_PROCESS_SHARED) < 0) {
      perror("Can't pthread_mutexattr_setpshared");
      throw std::runtime_error(std::to_string(errno));
    }
  }

  void initCondAttr() {
    if (pthread_condattr_init(&mCondAttr) < 0) {
      perror("Can't pthread_condattr_init");
      throw std::runtime_error(std::to_string(errno));
    }

    if (pthread_condattr_setpshared(&mCondAttr, PTHREAD_PROCESS_SHARED) < 0) {
      perror("Can't pthread_condattr_setpshared");
      throw std::runtime_error(std::to_string(errno));
    }
  }

public:
  explicit StateMachineManager(bool theIsServer) {
    mBufferResource = std::make_unique<StateMachineResource<char>>(
        SHM_BUFFER_SIZE, SHM_BUFFER_KEY, theIsServer);
    mSyncResource = std::make_unique<StateMachineResource<shm_sync>>(
        sizeof(shm_sync), SHM_SYNC_KEY, theIsServer);
  };

  ~StateMachineManager() { close(); }

  /**
   * This method opens shared memory
   */
  virtual void open() {
    mBufferResource->open();
    mSyncResource->open();
  }

  shm_sync *getSyncStructure() { return mSyncResource->getResource(); }

  bool *isTransferStarted() {
    return &mSyncResource->getResource()->mIsTransferStarted;
  }

  char *getTransferBuffer() { return mBufferResource->getResource(); }

  pthread_mutex_t *getMutex() { return &mSyncResource->getResource()->mMutex; }

  pthread_cond_t *getConditionalVariable() {
    return &mSyncResource->getResource()->mConditionalVariable;
  }

  size_t *getActiveClientCounter() {
    return &mSyncResource->getResource()->mActiveClients;
  }

  /**
   * This method closes shared memory
   */
  void close() {
    pthread_condattr_destroy(&mCondAttr);
    pthread_mutexattr_destroy(&mMutexAttr);
    mBufferResource->close();
    mSyncResource->close();
  }
};
