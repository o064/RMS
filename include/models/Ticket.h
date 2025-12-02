//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TICKET_H
#define RMS_TICKET_H

#include "Passenger.h"

enum Status
{
    booked,
    cancelled
};
class Ticket
{
    int id;
    int ticketSeat;
    int trainId;
    Passenger passenger;
    Status status;

public:
public:
    Ticket() = default;
    Ticket(const int id,const int seat, const int trainId,const Passenger& p);
    int getId() const;
    int getSeat() const;
    Status getStatus() const;
    void setStatus(const Status& s);
    int getTrainId() const;
    Passenger getPassenger() const;
    void setId(const int newId);
    void print(const std::string& msg) const ;

};
#endif // RMS_TICKET_H
