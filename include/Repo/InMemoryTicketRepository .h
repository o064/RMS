//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYTICKETREPOSITORY_H
#define RMS_INMEMORYTICKETREPOSITORY_H

#include <map>
#include <vector>
#include "ITicketRepository.h"
#include "../models/Ticket.h"

class InMemoryTicketRepository : public ITicketRepository {
private:
    std::map<int, Ticket> tickets;
public:
     Ticket* getTicketByTrainAndPassenger(int trainId, int passengerId) override;
     bool deleteTicket(int ticketId) override;
     Ticket save(Ticket ticket) override;
     std::vector<Ticket> getAllTickets() override;
     Ticket* getTicketById(int ticketId )override ;
    void clear();
};
#endif //RMS_INMEMORYTICKETREPOSITORY_H
