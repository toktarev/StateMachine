#pragma once

#include "client/ClientSharedMemoryResource.h"
#include "server/ServerSharedMemoryResource.h"
#include "shm.h"
#include <errno.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

template <typename T> class StateMachineResource {

private:
  bool mIsServer;

  int mResourceHandle{};

  T *mShareResource{nullptr};

  std::unique_ptr<SharedMemoryResource> mServerSharedMemoryResource{};

public:
  StateMachineResource(const size_t theSize, int theResourceHandle,
                       bool theIsServer)
      : mResourceHandle(theResourceHandle), mIsServer(theIsServer) {
    if (theIsServer) {
      mServerSharedMemoryResource =
          std::make_unique<ServerSharedMemoryResource>(theSize,
                                                       theResourceHandle);
    } else {
      mServerSharedMemoryResource =
          std::make_unique<ClientSharedMemoryResource>(theSize,
                                                       theResourceHandle);
    }
  };

  ~StateMachineResource() { close(); }

  T *getResource() { return mShareResource; }

  void open() {
    void *aResult = mServerSharedMemoryResource->open();
    mShareResource = (T *)aResult;
  }

  void close() { mServerSharedMemoryResource->close(); }
};