//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"
#include <stdexcept> //for run time exception
#include <iostream>
#include "utils/helpers.h"



RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps) {
    this->trainService= ts;
    this->ticketService=tks;
    this->passengerService=ps;
}

// ============ Trains =============
std::vector<Train> RMSFacade::listTrains() {
    return trainService->getAllTrains();
}

Train RMSFacade::addTrain( std::string& name, int totalSeats) {
    name = trim(name);
    if (name.empty()) {
        throw std::invalid_argument("Train name cannot be empty");
    }

    if (totalSeats <= 0) {
        throw std::invalid_argument("Total seats must be greater than 0");
    }

    return trainService->createTrain(name,totalSeats);
}

std::optional<Train> RMSFacade::getTrain(const int &trainId) {
    return trainService->getTrain(trainId);
}
// ============ Passengers =============

std::optional<Passenger> RMSFacade::getPassenger(const int &passengerId) {
    return passengerService->getPassenger(passengerId);
}


Passenger RMSFacade::addPassenger(std::string &name) {
    name = trim(name);
    if (name.empty()) {
        throw std::invalid_argument("Passenger name cannot be empty");
    }
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
    try {
        Passenger ps = passengerService->find_or_create_passenger(passengerName);
        return ticketService->bookTicket(trainId, ps.getId());
    } catch (const std::exception& e) {
        throw std::runtime_error(
                "Failed to book ticket for train " + std::to_string(trainId) +
                " and passenger '" + passengerName + "': " + e.what()
        );
    }
}

void RMSFacade::cancelTicket(const int &ticketId) {
    return ticketService->cancelTicket(ticketId);
}

bool RMSFacade::getTrainAvailability(const int &trainId) {
    auto train =trainService->getTrain(trainId);
    if(!train.has_value())
        throw std::out_of_range("train with id : " +  std::to_string(trainId) + "does not exist");

    auto allocator = train->getSeatAllocator();
    if (!allocator)  // null
        throw std::runtime_error(
                "Train with ID " + std::to_string(trainId) + " has no seat allocator"
        );
    return allocator->hasAvailableSeats();
}

Train RMSFacade::updateTrain( int trainId, const std::string &name,  int seats) {
    auto currentTrain = trainService->getTrain(trainId);
    if(!currentTrain.has_value())
        throw std::out_of_range("train with id : " +  std::to_string(trainId) + "does not exist");

    
    
    int currentSeats = currentTrain->getTotalSeats();
    // update train
    auto updatedTrain = trainService->updateTrain(trainId, name,seats);
//    book tickets for the waiting list
    if(currentSeats < seats){ // new Seats added
        auto waitingList = updatedTrain.getSeatAllocator()->getWaitingList();
        int seatsToAdd = seats-currentSeats;
        while (!waitingList.empty() && seatsToAdd >  0 ){
            int frontPassengerId = waitingList.front();

            try {
                ticketService->bookTicket(trainId, frontPassengerId);
                seatsToAdd--;
            } catch (const std::exception& e) {
                // log error and continue to the  remaining passengers
                std::cerr << "Failed to book ticket for waiting passenger "
                          << frontPassengerId << ": " << e.what() << "\n";
            }
        }
        trainService->save(updatedTrain);
    }

    return  updatedTrain;

}

Train RMSFacade::addSeats( int trainId,  int seats) {
    return trainService->addSeats(trainId,seats);
}

Train RMSFacade::addSeats(const std::string &name,  int seats) {
    return trainService->addSeats(name,seats);
}

Passenger RMSFacade::updatePassenger( int passengerId, const std::string &name) {
    auto passenger = passengerService->getPassenger(passengerId);
    if(!passenger.has_value())
        throw std::out_of_range("passenger with id : " + std::to_string(passengerId) + " does not exist ");
    //update passenger
    auto newPassenger = passengerService->updatePassenger(passengerId , name);
    // update tickets with this id
    auto tickets = ticketService->getAllTickets();
    for(auto & t : tickets){
        if(compareString(t.getPassenger().getName(),passenger->getName())){
            t.getPassenger().setName(name);
            ticketService->updateTicket(t);
        }
    }

    return newPassenger;
}

void RMSFacade::deleteTrain( int trainId) {
    bool deleted= trainService->deleteTrain(trainId);
    if(!deleted)
        throw std::out_of_range("failed to delete train with id : " + std::to_string(trainId));

}

void RMSFacade::deletePassenger( int passengerId) {
    bool deleted=passengerService->deletePassenger(passengerId);
    if(!deleted)
        throw std::out_of_range("failed to delete passenger with id : " + std::to_string(passengerId));
}


