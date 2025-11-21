//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYTICKETREPOSITORY_H
#define RMS_INMEMORYTICKETREPOSITORY_H

#include <map>
#include <vector>
#include "ITicketRepository.h"
#include "../models/Ticket.h"
#include <memory>

class InMemoryTicketRepository : public ITicketRepository
{
private:
    std::map<int, std::shared_ptr<Ticket>> tickets;

public:
    std::shared_ptr<Ticket> getTicketByTrainAndPassenger(int trainId, int passengerId) override;
    bool deleteTicket(int ticketId) override;
    Ticket save(Ticket ticket) override;
    std::vector<std::shared_ptr<Ticket>> getAllTickets() override;
    std::shared_ptr<Ticket> getTicketById(int ticketId) override;
    void clear();
};
#endif // RMS_INMEMORYTICKETREPOSITORY_H
