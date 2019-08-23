#pragma once

#include <StateMachineManager.h>
#include <pthread.h>

class ServerManager : public StateMachineManager {
private:
  void initMutex() {
    initMutexAttr();
    if (pthread_mutex_init(&mSyncResource->getResource()->mMutex, &mMutexAttr) <
        0) {
      perror("Can't pthread_mutex_init");
      throw std::runtime_error(std::to_string(errno));
    }
  }

  void initConditionalVariable() {
    initCondAttr();
    if (pthread_cond_init(&mSyncResource->getResource()->mConditionalVariable,
                          &mCondAttr) < 0) {
      perror("Can't pthread_cond_init");
      throw std::runtime_error(std::to_string(errno));
    }
  }

public:
  explicit ServerManager() : StateMachineManager(true){};

  void open() override {
    StateMachineManager::open();

    initMutex();
    initConditionalVariable();
  }
};