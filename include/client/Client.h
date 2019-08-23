#include <utility>

#pragma once

#include "StateMachine.h"

class Client : public StateMachine {

private:
  bool mIsTrigger;

  size_t mClientNumber;

  std::string mTargetDirectory;

public:
  explicit Client(bool theIsTrigger, std::string theTargetDirectory)
      : StateMachine(false) {
    mIsTrigger = theIsTrigger;
    mTargetDirectory = std::move(theTargetDirectory);
  };

  void awaitTransfer() override;

  void doTransfer() override;
};
