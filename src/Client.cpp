#include "client/Client.h"
#include "shm.h"
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <unistd.h>

void Client::awaitTransfer() {
    {
        plock_guard mGuard{mSharedMemoryManager->getMutex()};
        mClientNumber = *(mSharedMemoryManager->getActiveClientCounter());
        *(mSharedMemoryManager->getActiveClientCounter()) = mClientNumber + 1;
        std::cout << "Client with number " << mClientNumber << " joined"
                  << std::endl;
    }

    if (mIsTrigger) {
        std::cout << "Wake up all clients and server" << std::endl;
        wakeUpAll();
        mSharedMemoryManager->getSyncStructure()->mIsTransferStarted = true;
    } else {
        std::cout << "Client is waiting for client's signal" << std::endl;
        awaitAll();
    }
}

void Client::doTransfer() {
    std::cout << "Starting file's receiving " << std::endl;

    if (*(mSharedMemoryManager->isTransferStarted())) {
        std::string aTargetDirectory = mTargetDirectory;
        std::string aTargetFileName = "Target" + std::to_string(mClientNumber);
        std::string aTargetFile =
                aTargetDirectory.append("/").append(aTargetFileName);
        remove(aTargetFile.data());

        char *aBuffer = mSharedMemoryManager->getTransferBuffer();

        std::ofstream ofStream(aTargetFile);

        if (!ofStream.good()) {
            std::cout << "Can't write to target file " + aTargetFileName << std::endl;
        }

        size_t aBytesWritten = 0;

        do {
            do {
                // Sleep for 100ns to prevent CPU starvation
                usleep(SLEEP_MICROSECONDS);

                if (mSharedMemoryManager->getSyncStructure()->mServerTransferComplete) {
                    break;
                }

                {
                    plock_guard mGuard{mSharedMemoryManager->getMutex()};
                    if (mSharedMemoryManager->getSyncStructure()->mBytesReady !=
                        aBytesWritten) {
                        break;
                    }
                }
            } while (true);

            size_t aBytesAvailabie;

            {
                plock_guard mGuard{mSharedMemoryManager->getMutex()};
                aBytesAvailabie = mSharedMemoryManager->getSyncStructure()->mBytesReady;
            }

            while (aBytesAvailabie - aBytesWritten > 0) {
                size_t aOffset = ((size_t) (aBytesWritten)) % ((size_t) SHM_BUFFER_SIZE);

                size_t aBytesToWrite =
                        std::min((size_t) (SHM_BUFFER_SIZE - aOffset), (size_t) FILE_CHUNK);

                aBytesToWrite =
                        std::min(aBytesToWrite, aBytesAvailabie - aBytesWritten);

                ofStream.write(aBuffer + aOffset, aBytesToWrite);
                aBytesWritten += aBytesToWrite;
            }

            aBytesWritten = aBytesAvailabie;

            {
                plock_guard mGuard{mSharedMemoryManager->getMutex()};
                mSharedMemoryManager->getSyncStructure()->mReceivedClientsChunkNumber++;
            }
        } while (
                !mSharedMemoryManager->getSyncStructure()->mServerTransferComplete);

        std::cout << "Transfer completed, written: "
                  << std::to_string(aBytesWritten) << " bytes to " + aTargetFile
                  << std::endl;
    }
}