//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_RMSFACADE_H
#define RMS_RMSFACADE_H

#include <vector>
#include "Services/TicketService.h"
#include "Services/PassengerService.h"
#include "Services/TrainService.h"

class RMSFacade
{
private:
    TrainService *trainService;
    TicketService *ticketService;
    PassengerService *passengerService;

public:
    RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps);

    std::list<Train> listTrains();
    Train addTrain(const std::string &name, const int &totalSeats);
    Train getTrain(const int &trainId);
    Passenger getPassenger(const int &totalSeats);
    Passenger addPassenger(const std::string &name);
    std::vector<Ticket> listTickets();
    bool bookTicket(const int &totalSeats, const std::string &passengerName);
    bool cancelTicket(const int &totalSeats);
    bool getTrainAvailability(const int &totalSeats);
};
#endif // RMS_RMSFACADE_H
