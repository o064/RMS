#include <stdexcept>
#include <iostream>
#include <utility>
#include "models/Ticket.h"
using std::cout;
using std::endl;

Ticket::Ticket(const int id, const int seat, const int trainId, Passenger  p)
        :passenger(std::move(p)), status(booked){
    if(id <0 || trainId <= 0 ) throw std::invalid_argument("Invalid id");
    if(seat<=0) throw std::invalid_argument("Invalid seat");
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
    if(newId <=0 )throw std::invalid_argument("Invalid id");
    this->id = newId;
}

void Ticket::print(const std::string &msg) const {
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Ticket ID: " << id << "\n";
    cout << "Seat: " << ticketSeat<< "\n";
    cout << "Train ID: " << trainId << "\n";
    cout << "Passenger: " << passenger.getName() << "\n";
    cout << "Status : " << ((status == Status::booked) ? "Booked" : "Cancelled") << "\n";
    cout << "--------------------------------------------------\n";
}

void Ticket::setPassenger(const Passenger &p) {
    this->passenger =p ;

}

