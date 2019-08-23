#include "client/ClientSharedMemoryResource.h"
#include "shm.h"
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

int ClientSharedMemoryResource::createSharedMemoryId(key_t theKey) {
  int aSharedMemoryId = shmget(theKey, mSize, IPC_NOWAIT);

  if (aSharedMemoryId < 0) {
    if (errno == 2) {
      throw std::runtime_error("Server hasn't been started");
    }

    perror("Unable to shmget");
    throw std::runtime_error(std::to_string(errno));
  }

  return aSharedMemoryId;
};

void ClientSharedMemoryResource::close() {
  if (mSharedMemoryOpened) {
    SharedMemoryResource::close();
    mSharedMemoryOpened = false;
  }
};