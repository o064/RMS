//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"




RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps) {
    this->trainService= ts;
    this->ticketService=tks;
    this->passengerService=ps;
}

// ============ Trains =============
std::list<Train> RMSFacade::listTrains() {
    return trainService->getAllTrains();
}

Train RMSFacade::addTrain(const std::string& name,const int& totalSeats) {
    return trainService->createTrain(name,totalSeats);
}

std::optional<Train> RMSFacade::getTrain(const int &trainId) {
    return trainService->getTrain(trainId);
}
// ============ Passengers =============

std::optional<Passenger> RMSFacade::getPassenger(const int &passengerId) {
    return passengerService->getPassenger(passengerId);
}


Passenger RMSFacade::addPassenger(const std::string &name) {
    return passengerService->createPassenger(name);
}

std::list<Ticket> RMSFacade::listTickets() {
    return ticketService->getAllTickets();

}

std::list<Passenger> RMSFacade::listPassenger() {
    return passengerService->getAllPassengers();

}

// ============ Tickets =============
Ticket RMSFacade::bookTicket(const int &trainId, const int& passengerId) {
    return ticketService->bookTicket(trainId,passengerId);
}

void RMSFacade::cancelTicket(const int &ticketId) {
    return ticketService->cancelTicket(ticketId);
}

bool RMSFacade::getTrainAvailability(const int &trainId) {
    return trainService->isAvailbleSeat(trainId);
}

