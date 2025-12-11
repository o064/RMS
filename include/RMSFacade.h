//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_RMSFACADE_H
#define RMS_RMSFACADE_H

#include "./structures/vector.h"
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
    vector<Train> listTrains();
    Train addTrain(std::string name,  int totalSeats);
    Train getTrain(int trainId);
    bool getTrainAvailability(int trainId);
    Train updateTrain(int trainId, const std::string& name  ,int seats=0);
    Train addSeats(int trainId,int seats=0);
    Train addSeats(const std::string& name,int seats=0);
    void deleteTrain(int trainId);
    void trainStatus(int trainId);

    // passenger features
    vector<Passenger> listPassengers();
    Passenger getPassenger(int totalSeats);
    Passenger addPassenger( const std::string& name);
    Passenger updatePassenger(int passengerId, const std::string &name);
    void deletePassenger(int passengerId);


    // ticket features
    vector<Ticket> listTickets();
    std::optional<Ticket> bookTicket(int trainId, const std::string& passengerName);
    void cancelTicket(int ticketId);


};
#endif // RMS_RMSFACADE_H
