//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TICKETSERVICE_H
#define RMS_TICKETSERVICE_H

#include "../Repo/ITicketRepository.h"

class TicketService
{
private:
    ITicketRepository *ticketRepository;

public:
    TicketService(ITicketRepository *repo);
    Ticket getTicket(int ticketId);
    std::vector<Ticket> getAllTickets();
    Ticket getTicketByTrainAndPassenger(int trainId, int passengerId);
    Ticket bookTicket(int trainId, int passengerId);
    bool cancelTicket(int ticketId);
};
#endif // RMS_TICKETSERVICE_H
