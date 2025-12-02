//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"
#include <stdexcept> //for run time exception
#include <iostream>
#include "utils/helpers.h"

RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps)
{
    this->trainService = ts;
    this->ticketService = tks;
    this->passengerService = ps;
}

// ============ Trains =============
std::vector<Train> RMSFacade::listTrains()
{
    return trainService->getAllTrains();
}

Train RMSFacade::addTrain(std::string name, int totalSeats)
{
    name = trim(name);
    if (name.empty())
    {
        throw std::invalid_argument("Train name cannot be empty");
    }

    if (totalSeats <= 0)
    {
        throw std::invalid_argument("Total seats must be greater than 0");
    }

    return trainService->createTrain(name, totalSeats);
}

std::optional<Train> RMSFacade::getTrain(int trainId)
{
    return trainService->getTrain(trainId);
}
// ============ Passengers =============

std::optional<Passenger> RMSFacade::getPassenger(int passengerId)
{
    return passengerService->getPassenger(passengerId);
}

Passenger RMSFacade::addPassenger(const std::string& name)
{
    if (!isValidName(name))
        throw std::invalid_argument("Passenger name cannot be empty");
    std::string trimmedName = trim(name);
    
    return passengerService->createPassenger(trimmedName);
}

std::vector<Ticket> RMSFacade::listTickets()
{
    return ticketService->getAllTickets();
}

std::vector<Passenger> RMSFacade::listPassengers()
{
    return passengerService->getAllPassengers();
}

// ============ Tickets =============
std::optional<Ticket> RMSFacade::bookTicket(int trainId, const std::string &passengerName)
{
    // input validation
    if (trainId <= 0)
        throw std::invalid_argument("Train ID must be greater than 0");

    if (!isValidName(passengerName))
        throw std::invalid_argument("Passenger name cannot be empty");
    std::string trimmedName = trim(passengerName);


    Passenger ps = passengerService->find_or_create_passenger(passengerName);
    return ticketService->bookTicket(trainId, ps.getId());

}

void RMSFacade::cancelTicket(int ticketId)
{
    // input validation

    if (ticketId <= 0)
        throw std::invalid_argument("Ticket ID must be greater than 0");

    ticketService->cancelTicket(ticketId);


}

bool RMSFacade::getTrainAvailability(int trainId)
{
    return  trainService->isAvailbleSeat(trainId);

}

Train RMSFacade::updateTrain(int trainId, const std::string& name, int seats) {
    // validation
    if (trainId <= 0) throw std::invalid_argument("Train ID must be > 0");
    std::string trimmedName = trim(name);
    if (!isValidName(trimmedName)) throw std::invalid_argument("Train name cannot be empty");
    if (seats < 0) throw std::invalid_argument("Seats cannot be negative");

    // current Train
    auto currentTrainOpt = trainService->getTrain(trainId);
    if (!currentTrainOpt.has_value())
        throw std::out_of_range("Train with id " + std::to_string(trainId) + " does not exist");

    Train currentTrain = currentTrainOpt.value();
    int currentSeats = currentTrain.getTotalSeats();

    // update train
    Train updatedTrain = trainService->updateTrain(trainId, trimmedName, seats);

    // process waiting
    int seatsAdded = seats - currentSeats;
    if (seatsAdded > 0) {
        auto cb =[this, trainId](int passengerId){ // access to this object& trainId + passengerId as argument
            ticketService->bookTicket(trainId, passengerId);
        };
        updatedTrain.getSeatAllocator()->processWaitingList(seatsAdded,cb);

        // Save updated train after processing waiting list
        trainService->save(updatedTrain);
    }

    return updatedTrain;
}

Train RMSFacade::addSeats(int trainId, int seats)
{
    return trainService->addSeats(trainId, seats);
}

Train RMSFacade::addSeats(const std::string &name, int seats)
{
    return trainService->addSeats(name, seats);
}

Passenger RMSFacade::updatePassenger(int passengerId, const std::string &name)
{
    auto passenger = passengerService->getPassenger(passengerId);
    if (!passenger.has_value())
        throw std::out_of_range("passenger with id : " + std::to_string(passengerId) + " does not exist ");
    // update passenger
    auto newPassenger = passengerService->updatePassenger(passengerId, name);
    // update tickets with this id
    auto tickets = ticketService->getAllTickets();
    for (auto &t : tickets)
    {
        if (compareString(t.getPassenger().getName(), passenger->getName()))
        {
            t.getPassenger().setName(name);
            ticketService->updateTicket(t);
        }
    }

    return newPassenger;
}

void RMSFacade::deleteTrain(int trainId)
{
    bool deleted = trainService->deleteTrain(trainId);
    if (!deleted)
        throw std::out_of_range("failed to delete train with id : " + std::to_string(trainId));
}

void RMSFacade::deletePassenger(int passengerId)
{
    bool deleted = passengerService->deletePassenger(passengerId);
    if (!deleted)
        throw std::out_of_range("failed to delete passenger with id : " + std::to_string(passengerId));
}

void RMSFacade::trainStatus(int trainId) {
    trainService->printStatus(trainId);
}
