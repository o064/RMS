//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_STARTUPMANAGER_H
#define RMS_STARTUPMANAGER_H

#include "RMSFacade.h"

class StartupManager {
private:
    ITrainRepository* trainRepository;
    ITicketRepository* ticketReposatory;
    IPassengerRepository* passengerReposatory;
    TrainService* trainService;
    PassengerService* passengerService;
    TicketService* TicketService;

    RMSFacade* facade;
public:
    RMSFacade * buildFacade() ;


};
#endif //RMS_STARTUPMANAGER_H
