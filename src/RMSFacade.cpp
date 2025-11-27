//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"
#include <stdexcept> //for run time exception



RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps) {
    this->trainService= ts;
    this->ticketService=tks;
    this->passengerService=ps;
}

// ============ Trains =============
std::vector<Train> RMSFacade::listTrains() {
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

std::vector<Ticket> RMSFacade::listTickets() {
    return ticketService->getAllTickets();

}

std::vector<Passenger> RMSFacade::listPassengers() {
    return passengerService->getAllPassengers();

}

// ============ Tickets =============
Ticket RMSFacade::bookTicket(const int &trainId, const std::string& passengerName) {

    Passenger ps = passengerService->find_or_create_passenger(passengerName);
    return ticketService->bookTicket(trainId,ps.getId());
}

void RMSFacade::cancelTicket(const int &ticketId) {
    return ticketService->cancelTicket(ticketId);
}

bool RMSFacade::getTrainAvailability(const int &trainId) {
    auto train =trainService->getTrain(trainId);
    if(!train.has_value()){
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");
    }
    auto allocator = train->getSeatAllocator();
    if(allocator !=nullptr){
        return  allocator->hasAvailableSeats();
    }
    throw std::runtime_error("train with id : " +  std::to_string(trainId) + "do not have seat allocator");


    return trainService->isAvailbleSeat(trainId);
}

