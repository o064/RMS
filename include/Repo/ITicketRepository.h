//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_ITICKETREPOSITORY_H
#define RMS_ITICKETREPOSITORY_H

#include <vector>
#include "../models/Ticket.h"

class ITicketRepository
{
public:
    virtual std::shared_ptr<Ticket> getTicketByTrainAndPassenger(int trainId, int passengerId) = 0;
    virtual bool deleteTicket(int ticketId) = 0;
    virtual Ticket save(Ticket ticket) = 0;
    virtual std::vector<std::shared_ptr<Ticket>> getAllTickets() = 0;
    virtual std::shared_ptr<Ticket> getTicketById(int) = 0;
    virtual void clear() = 0;

    virtual ~ITicketRepository() = default;
};
#endif // RMS_ITICKETREPOSITORY_H
