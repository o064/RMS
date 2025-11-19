//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TICKET_H
#define RMS_TICKET_H

#include "Passenger.h"

enum Status {
    booked,
    cancelled
};
class Ticket {
    int id;
    int ticketSeat;
    int trainId;
    Passenger passenger;
    Status status;
public:
    Ticket(int id, int seat, int trainId, Passenger p);
    int getId() const;
    int getSeat() const;
    Status getStatus() const;
    void setStatus(Status s);
};
#endif //RMS_TICKET_H
