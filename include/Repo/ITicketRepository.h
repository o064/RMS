//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_ITICKETREPOSITORY_H
#define RMS_ITICKETREPOSITORY_H

#include <vector>
#include <optional>

#include "../models/Ticket.h"

class ITicketRepository
{
public:
    virtual std::optional<Ticket> getTicketByTrainAndPassenger(int trainId, int passengerId) = 0;
    virtual bool deleteTicket(int ticketId) = 0;
    virtual void save(Ticket& ticket) = 0;
    virtual std::vector<Ticket> getAllTickets() = 0;
    virtual std::optional<Ticket> getTicketById(int) = 0;
    virtual void clear() = 0;

    virtual ~ITicketRepository() = default;
};
#endif // RMS_ITICKETREPOSITORY_H
