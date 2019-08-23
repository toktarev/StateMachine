#pragma once

#include <iostream>
#include <memory>
#include <pthread.h>
#include <string>

class ifstream_guard final {
  std::unique_ptr<std::ifstream> mIfStream{};

public:
  explicit ifstream_guard(std::string theFileName) {
    mIfStream = std::make_unique<std::ifstream>();
    mIfStream.get()->open(theFileName, std::ios::in | std::ios::binary);
  }

  std::ifstream *getFStream() { return mIfStream.get(); }

  ~ifstream_guard() {
    if (mIfStream) {
      mIfStream.get()->close();
    }
  }
};