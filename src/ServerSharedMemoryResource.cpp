#include "server/ServerSharedMemoryResource.h"
#include "shm.h"
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

int ServerSharedMemoryResource::createSharedMemoryId(key_t theKey) {
  int aSharedMemoryId =
      shmget(theKey, mSize, IPC_CREAT | IPC_EXCL | SHM_PERMISSION);

  if ((aSharedMemoryId < 0) && (EEXIST == errno)) {
    aSharedMemoryId = shmget(theKey, mSize, IPC_CREAT | SHM_PERMISSION);

    if (shmctl(aSharedMemoryId, IPC_RMID, nullptr) < 0) {
      perror("Unable to shmctl");
      throw std::runtime_error(std::to_string(errno));
    }

    aSharedMemoryId =
        shmget(theKey, mSize, IPC_CREAT | IPC_EXCL | SHM_PERMISSION);
  }

  if (aSharedMemoryId < 0) {
    perror("Unable to shmget");
    throw std::runtime_error(std::to_string(errno));
  }

  return aSharedMemoryId;
};

void ServerSharedMemoryResource::close() {
  if (mSharedMemoryOpened) {
    SharedMemoryResource::close();

    if (shmctl(mSharedMemoryId, IPC_RMID, nullptr) < 0) {
      perror("Unable to shmctl");
    }

    mSharedMemoryOpened = false;
  }
};