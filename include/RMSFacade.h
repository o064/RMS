//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_RMSFACADE_H
#define RMS_RMSFACADE_H

#include <vector>
#include "Services/TicketService.h"
#include "Services/PassengerService.h"
#include "Services/TrainService.h"

class RMSFacade{
private:
    TrainService* trainService;
    TicketService* ticketService;
    PassengerService* passengerService;

public:
    RMSFacade(TrainService* ts, TicketService* tks, PassengerService* ps);

    std::vector<Train> listTrains();
    bool addTrain(std::string name, int totalSeats);
    Train* getTrain(int trainId);
    Passenger* getPassenger(int passengerId);
    Passenger addPassenger(std::string name);
    std::vector<Ticket> listTickets();
    bool bookTicket(int trainId, std::string passengerName);
    bool cancelTicket(int ticketId);
    bool getTrainAvailability(int trainId);
    void save_all_data(std::vector<Train> trains, std::vector<Ticket> tickets);
    void load_all_data();
private:
    void save_trains(std::vector<Train> trains);
    void save_passengers(std::vector<Passenger> passengers);
    void save_tickets(std::vector<Ticket> tickets);
};
#endif //RMS_RMSFACADE_H
