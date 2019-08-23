#pragma once

#include <pthread.h>

#define IPC_KEY_FILE "/"

#define SHM_PERMISSION 0660

#define SHM_BUFFER_KEY 16

#define SHM_SYNC_KEY 32

#define SHM_BUFFER_SIZE 1024 * 1024

#define FILE_CHUNK 1024

#define SLEEP_MICROSECONDS 100

typedef struct {
  bool mIsTransferStarted{false};
  bool mServerTransferComplete{false};
  pthread_mutex_t mMutex{};
  pthread_cond_t mConditionalVariable{};
  size_t mActiveClients{};
  size_t mReceivedClientsChunkNumber{};
  size_t mBytesReady = {0};
} shm_sync;
