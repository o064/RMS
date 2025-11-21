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

    std::vector<std::shared_ptr<Train>> listTrains();
    std::shared_ptr<Train> addTrain(const std::string &name, const int &totalSeats);
    std::shared_ptr<Train> getTrain(const int &trainId);
    std::shared_ptr<Passenger> getPassenger(const int &totalSeats);
    std::shared_ptr<Passenger> addPassenger(const std::string &name);
    std::vector<std::shared_ptr<Ticket>> listTickets();
    bool bookTicket(const int &totalSeats, const std::string &passengerName);
    bool cancelTicket(const int &totalSeats);
    bool getTrainAvailability(const int &totalSeats);
};
#endif // RMS_RMSFACADE_H
