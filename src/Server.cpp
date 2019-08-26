#include "server/Server.h"
#include "shm.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <ifstream_guard.h>
#include <inttypes.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void Server::awaitTransfer() {
  std::cout << "Server is waiting for client's signal" << std::endl;
  awaitAll();
  mActiveClientsNumber = *(mSharedMemoryManager->getActiveClientCounter());
}

void Server::doTransfer() {
  std::cout << "Starting file's transfer " << std::endl;

  mSharedMemoryManager->getSyncStructure()->mBytesReady = 0;
  uint64_t mStartTransferTime =
      (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  mSharedMemoryManager->getSyncStructure()->mServerTransferComplete = false;

  char *aBuffer = mSharedMemoryManager->getTransferBuffer();
  ifstream_guard aStreamGuard(mFileName);

  if (!aStreamGuard.getFStream()->good()) {
    std::cout << "Can't read file " << mFileName;
    *(mSharedMemoryManager->isTransferStarted()) = false;
  }

  while (!aStreamGuard.getFStream()->eof()) {
    {
      plock_guard mGuard{mSharedMemoryManager->getMutex()};
      mSharedMemoryManager->getSyncStructure()->mReceivedClientsChunkNumber = 0;
    }

    size_t aReadBytes = 0;

    while (aReadBytes < SHM_BUFFER_SIZE) {
      size_t aOffset =
          ((size_t)(mSharedMemoryManager->getSyncStructure()->mBytesReady +
                    aReadBytes)) %
          ((size_t)SHM_BUFFER_SIZE);

      aStreamGuard.getFStream()->read(
          aBuffer + aOffset,
          std::min((size_t)(SHM_BUFFER_SIZE - aOffset), (size_t)FILE_CHUNK));

      if (aStreamGuard.getFStream()->gcount() == 0) {
        break;
      }

      aReadBytes += aStreamGuard.getFStream()->gcount();
    }

    {
      plock_guard mGuard{mSharedMemoryManager->getMutex()};
      mSharedMemoryManager->getSyncStructure()->mBytesReady += aReadBytes;
    }

    do {
      // Sleep for 100ns to prevent CPU starvation
      usleep(SLEEP_MICROSECONDS);
    } while (
        mSharedMemoryManager->getSyncStructure()->mReceivedClientsChunkNumber <
        mActiveClientsNumber);

    if (aStreamGuard.getFStream()->eof()) {
      mSharedMemoryManager->getSyncStructure()->mServerTransferComplete = true;
      break;
    }
  }

  uint64_t mEndTransferTime =
      (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();

  u_int64_t aDelta = mEndTransferTime - mStartTransferTime;

  printf("Transfer completed, sent %" PRIu64 " bytes Time: %" PRIu64
         " milliseconds\n",
         (u_int64_t)mSharedMemoryManager->getSyncStructure()->mBytesReady,
         aDelta);

  if (!mSharedMemoryManager->getSyncStructure()->mServerTransferComplete) {
    mSharedMemoryManager->getSyncStructure()->mServerTransferComplete = true;
  }
}
