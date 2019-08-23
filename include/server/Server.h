#include <utility>

#pragma once

#include "StateMachine.h"

class Server : public StateMachine {
private:
  std::string mFileName;

  size_t mActiveClientsNumber;

public:
  Server(std::string theFileName) : StateMachine(true) {
    mFileName = std::move(theFileName);
  };

  void awaitTransfer() override;

  void doTransfer() override;
};
