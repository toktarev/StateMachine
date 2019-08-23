#pragma once

#include "SharedMemoryResource.h"

class ServerSharedMemoryResource : public SharedMemoryResource {
public:
  ServerSharedMemoryResource(const size_t theSize, int theResourceHandle)
      : SharedMemoryResource(theSize, theResourceHandle) {}

  int createSharedMemoryId(key_t theKey) override;

  void close() override;
};