//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_RMSFACADE_H
#define RMS_RMSFACADE_H

#include <vector>
#include <optional>
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

    // train features
    std::vector<Train> listTrains();
    Train addTrain(const std::string &name, const int &totalSeats);
    std::optional<Train> getTrain(const int &trainId);
    bool getTrainAvailability(const int &trainId);

    // passenger features
    std::vector<Passenger> listPassengers();
    std::optional<Passenger> getPassenger(const int &totalSeats);
    Passenger addPassenger(const std::string &name);

    // ticket features
    std::vector<Ticket> listTickets();
    Ticket bookTicket(const int &trainId, const std::string& passengerName);
    void cancelTicket(const int &ticketId);


};
#endif // RMS_RMSFACADE_H
