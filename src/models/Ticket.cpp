#include <stdexcept>
#include "models/Ticket.h"

Ticket::Ticket(const int id, const int seat, const int trainId, const Passenger& p)
        :passenger(p), status(booked){
    if(id <0 || trainId < 0 ) throw std::runtime_error("Invalid id");
    if(seat<=0  ) throw std::runtime_error("Invalid seat");
    this->id = id;
    this->ticketSeat = seat;
    this->trainId = trainId;
}
int Ticket::getId() const
{
    return id;
}

int Ticket::getSeat() const
{
    return this->ticketSeat;
}

Status Ticket::getStatus() const
{
    return status;
}

void Ticket::setStatus(const Status& s)
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

void Ticket::setId(const int newId)
{
    this->id = newId;
}


