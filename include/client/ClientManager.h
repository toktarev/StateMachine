#pragma once
#include <StateMachineManager.h>
#include <pthread.h>

class ClientManager : public StateMachineManager {
public:
  explicit ClientManager() : StateMachineManager(false){};

  void open() override {
    StateMachineManager::open();
    initMutexAttr();
    initCondAttr();
  }
};