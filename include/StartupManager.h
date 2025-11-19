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
    IPassengerReposatory* passengerReposatory;
    TrainService* trainService;
    PassengerService* passengerService;
    TicketService* TicketService;

    RMSFacade* facade;


public:
    RMSFacade * buildFacade() ;
    void load_data_from_memory();
    void save_data_from_memory();

};
#endif //RMS_STARTUPMANAGER_H
