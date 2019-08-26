#include "client/Client.h"
#include "server/Server.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring>
#include <stdio.h>

void printHelp() {
  std::cout << " Server: \n"
               "       ./StateMachine -server <source_file_name> \n"
               "  Client \n"
               "       ./StateMachine -client <target_directory> [-start] \n"
            << std::endl;
}

int main(int argc, char *argv[]) {
  bool aIsServer;

  if (argc == 1) {
    std::cout << "Can' detect server or client option use: [./StateMachine -h]"
              << std::endl;
    exit(0);
  }

  if (strcmp(argv[1], "-h") == 0) {
    printHelp();
    exit(0);
  }

  if (strcmp(argv[1], "-server") == 0) {
    aIsServer = true;
  } else if (strcmp(argv[1], "-client") == 0) {
    aIsServer = false;
  } else {
    std::cout << "Can' detect server or client option [./StateMachine -h]"
              << std::endl;
    exit(0);
  }

  std::unique_ptr<StateMachine> aStateMachine;

  if (aIsServer) {
    if (argc == 2) {
      std::cout << "FileName hasn't been specified [./StateMachine -h]"
                << std::endl;
      exit(0);
    }

    std::string aFileName = argv[2];
    aStateMachine = std::make_unique<Server>(aFileName);
  } else {
    if (argc == 2) {
      std::cout << "Target directory hasn't been specified [./StateMachine -h]"
                << std::endl;
      exit(0);
    }

    std::string aTargetDirectory = argv[2];

    bool aStartTransfer;

    if (argc > 3) {
      aStartTransfer = strcmp(argv[3], "-start") == 0;
    } else {
      aStartTransfer = false;
    }

    aStateMachine = std::make_unique<Client>(aStartTransfer,aTargetDirectory);
  }

  std::cout << "Running " << (aIsServer ? "Server" : "Client") << std::endl;
  aStateMachine->run();

  std::cout << "Shutting down" << std::endl;
}
