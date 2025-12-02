//
// Created by Omar on 12/1/2025.
//

#include "RMSApp.h"

RMSApp::RMSApp() {
    startupManager = std::make_unique<StartupManager>();

    auto facade = startupManager->buildFacade(); // build the app with startup manager

    cli = std::make_unique<CLIController>(facade);// pass facade to startup manager
}

void RMSApp::run() {
    cli->run();
}
