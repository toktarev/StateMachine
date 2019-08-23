#pragma once

#include "SharedMemoryResource.h"

class ClientSharedMemoryResource : public SharedMemoryResource {
public:
  ClientSharedMemoryResource(const size_t theSize, int theResourceHandle)
      : SharedMemoryResource(theSize, theResourceHandle) {}

  int createSharedMemoryId(key_t theKey) override;

  void close() override;
};