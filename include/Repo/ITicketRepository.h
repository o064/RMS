//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_ITICKETREPOSITORY_H
#define RMS_ITICKETREPOSITORY_H

#include <vector>
#include "../models/Ticket.h"

class ITicketRepository {
public:
    virtual Ticket* getTicketByTrainAndPassenger(int trainId, int passengerId) = 0;
    virtual bool deleteTicket(int ticketId) = 0;
    virtual Ticket save(Ticket ticket) = 0;
    virtual std::vector<Ticket> getAllTickets() = 0;
    virtual  Ticket* getTicketById(int ) = 0 ;

    virtual ~ITicketRepository() = default;
};
#endif //RMS_ITICKETREPOSITORY_H
