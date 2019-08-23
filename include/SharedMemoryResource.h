#pragma once
#include "shm.h"
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

class SharedMemoryResource {
protected:
  size_t mSize;

  void *mShareResource;

  int mResourceHandle{};

  int mSharedMemoryId{};

  bool mSharedMemoryOpened{false};

  key_t createKey() {
    key_t aKey = ftok(IPC_KEY_FILE, mResourceHandle);

    if (aKey == (key_t)-1) {
      throw std::runtime_error("Unable to ftok");
    }

    return aKey;
  }

  void attach() {
    mShareResource = shmat(mSharedMemoryId, nullptr, SHM_PERMISSION);

    if (reinterpret_cast<long>(mShareResource) < 0) {
      perror("Unable to shmat");
      throw std::runtime_error(std::to_string(errno));
    }
  }

  virtual int createSharedMemoryId(key_t theKey) = 0;

public:
  SharedMemoryResource(const size_t theSize, int theResourceHandle)
      : mSize(theSize), mResourceHandle(theResourceHandle),
        mShareResource(nullptr) {}

  void *open() {
    key_t aKey = createKey();
    mSharedMemoryId = createSharedMemoryId(aKey);
    attach();
    mSharedMemoryOpened = true;
    return mShareResource;
  };

  virtual void close() {
    if (mSharedMemoryOpened) {
      if (shmdt(mShareResource) < 0) {
        perror("Unable to shmdt");
      }
    }
  }

  virtual ~SharedMemoryResource() { close(); }
};