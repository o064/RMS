//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYTICKETREPOSITORY_H
#define RMS_INMEMORYTICKETREPOSITORY_H

#include <map>
#include <vector>
#include <optional>

#include "ITicketRepository.h"
#include "../models/Ticket.h"

class InMemoryTicketRepository : public ITicketRepository
{
private:
    std::map<int, Ticket> tickets;
    int next_id = 1;

public:
    InMemoryTicketRepository() = default;
    ~InMemoryTicketRepository() override = default;
    std::optional<Ticket> getTicketByTrainAndPassenger(int trainId, int passengerId) override;
    bool deleteTicket(int ticketId) override;
    void save(Ticket& ticket) override;
    std::vector<Ticket> getAllTickets() override;
    std::optional<Ticket> getTicketById(int ticketId) override;
    void clear() override;
};
#endif // RMS_INMEMORYTICKETREPOSITORY_H
