#pragma once

#include "StateMachineManager.h"
#include "client/ClientManager.h"
#include "pthread_lock_guard.h"
#include "server/ServerManager.h"
#include <memory>
#include <unistd.h>

class StateMachine {

protected:
  std::unique_ptr<StateMachineManager> mSharedMemoryManager;

  void wakeUpAll() {
#ifndef __APPLE__
    {
      plock_guard mGuard{mSharedMemoryManager->getMutex()};
      pthread_cond_broadcast(mSharedMemoryManager->getConditionalVariable());
    }
#endif
  }

  void awaitAll() {
    while (!(*(mSharedMemoryManager->isTransferStarted()))) {
#ifdef __APPLE__
      /**
       * Pthread pthread_cond_wait doesn't work correctly for MAC
       */
      usleep(SLEEP_MICROSECONDS);
#else
      {
        plock_guard mGuard{mSharedMemoryManager->getMutex()};

        if (pthread_cond_wait(mSharedMemoryManager->getConditionalVariable(),
                              mSharedMemoryManager->getMutex()) != 0) {
          perror("pthread_cond_wait");
        }

        std::cout << "Client Connected "
                  << *(mSharedMemoryManager->getActiveClientCounter())
                  << std::endl;
      }
#endif
    }
  }

public:
  explicit StateMachine(bool theIsServer) {
    if (theIsServer) {
      mSharedMemoryManager = std::make_unique<ServerManager>();
    } else {
      mSharedMemoryManager = std::make_unique<ClientManager>();
    }
  }

  void run() {
    mSharedMemoryManager->open();
    awaitTransfer();
    doTransfer();
  }

  virtual void awaitTransfer() = 0;

  virtual void doTransfer() = 0;

  virtual ~StateMachine() = default;
};
