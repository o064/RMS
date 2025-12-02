//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TICKETSERVICE_H
#define RMS_TICKETSERVICE_H

#include "../Repo/ITicketRepository.h"
#include "TrainService.h"
#include "PassengerService.h"

class TicketService
{
private:
    ITicketRepository *ticketRepository;
    TrainService *trainService;
    PassengerService *passengerService;


public:
    TicketService(ITicketRepository *repo , TrainService* ts,PassengerService* ps);
    std::optional<Ticket> getTicket(const int& ticketId);
    std::vector<Ticket> getAllTickets();
    Ticket updateTicket(Ticket &t);

    std::optional<Ticket> getTicketByTrainAndPassenger(const int& trainId, const int& passengerId);
    std::optional<Ticket> bookTicket(const int& trainId, const int& passengerId);
    void cancelTicket(const int& ticketId);
};
#endif // RMS_TICKETSERVICE_H
