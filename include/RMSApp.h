//
// Created by Omar on 11/18/2025.
//

#ifndef RMS_RMSAPP_H
#define RMS_RMSAPP_H

#include "StartupManager.h"
#include "CLIController.h"

class RMSApp {
    StartupManager* startupManager;
    CLIController* cli;
public:
    void run();
    void shut_down();
};
#endif //RMS_RMSAPP_H
