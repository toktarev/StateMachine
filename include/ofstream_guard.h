#pragma once

#include <iostream>
#include <memory>
#include <pthread.h>
#include <string>

class ofstream_guard final {
    std::unique_ptr<std::ofstream> mIfStream{};

public:
    explicit ofstream_guard(std::string theFileName) {
        mIfStream = std::make_unique<std::ofstream>();
        mIfStream.get()->open(theFileName, std::ios::out | std::ios::binary);
    }

    std::ofstream *getFStream() { return mIfStream.get(); }

    ~ofstream_guard() {
        if (mIfStream) {
            mIfStream.get()->close();
        }
    }
};