#include "models/Ticket.h"

Ticket::Ticket(int id, int seat, int trainId, Passenger p)
{
    this->id = id;
    this->ticketSeat = seat;
    this->trainId = trainId;
    this->passenger = p;
    this->status = booked;
}

int Ticket::getId() const
{
    return id;
}

int Ticket::getSeat() const
{
    return ticketSeat;
}

Status Ticket::getStatus() const
{
    return status;
}

void Ticket::setStatus(Status s)
{
    this->status = s;
}

int Ticket::getTrainId() const
{
    return trainId;
}

Passenger Ticket::getPassenger() const
{
    return passenger;
}

void Ticket::setId(int newId)
{
    this->id = newId;
}
