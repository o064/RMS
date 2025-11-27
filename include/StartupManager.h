//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_STARTUPMANAGER_H
#define RMS_STARTUPMANAGER_H

#include "RMSFacade.h"
#include <memory>

class StartupManager {
private:
    std::unique_ptr<ITrainRepository> trainRepository;
    std::unique_ptr<ITicketRepository> ticketRepository;
    std::unique_ptr<IPassengerRepository> passengerRepository;
    std::unique_ptr<TrainService> trainService;
    std::unique_ptr<PassengerService> passengerService;
    std::unique_ptr<TicketService> ticketService;

    std::unique_ptr<RMSFacade> facade;
public:
    RMSFacade * buildFacade() ;

};
#endif //RMS_STARTUPMANAGER_H
