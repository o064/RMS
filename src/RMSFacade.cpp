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

std::optional<Train> RMSFacade::getTrain(const int &trainId)
{
    return trainService->getTrain(trainId);
}
// ============ Passengers =============

std::optional<Passenger> RMSFacade::getPassenger(const int &passengerId)
{
    return passengerService->getPassenger(passengerId);
}

Passenger RMSFacade::addPassenger(std::string name)
{
    name = trim(name);
    if (name.empty())
    {
        throw std::invalid_argument("Passenger name cannot be empty");
    }
    return passengerService->createPassenger(name);
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
Ticket RMSFacade::bookTicket(const int &trainId, const std::string &passengerName)
{
    if (trainId <= 0)
        throw std::invalid_argument("Train ID must be greater than 0");

    std::string trimmedName = trim(passengerName);
    if (trimmedName.empty())
        throw std::invalid_argument("Passenger name cannot be empty");
    // Verify train exists and has available seats
    auto trainOpt = trainService->getTrain(trainId);
    if (!trainOpt.has_value())
    {
        throw std::out_of_range(
            "Train with ID " + std::to_string(trainId) + " does not exist");
    }

    // Check train availability
    if (!getTrainAvailability(trainId))
    {
        throw std::runtime_error(
            "Train with ID " + std::to_string(trainId) +
            " has no available seats (passenger added to waiting list)");
    }
    try
    {
        Passenger ps = passengerService->find_or_create_passenger(passengerName);
        return ticketService->bookTicket(trainId, ps.getId());
    }
    catch (const std::invalid_argument &e)
    {
        throw std::invalid_argument(
            "Invalid booking data: " + std::string(e.what()));
    }
    catch (const std::out_of_range &e)
    {
        throw std::out_of_range(
            "Booking failed - resource not found: " + std::string(e.what()));
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(
            "Failed to book ticket for train " + std::to_string(trainId) +
            " and passenger '" + trimmedName + "': " + std::string(e.what()));
    }
}

void RMSFacade::cancelTicket(const int &ticketId)
{
    if (ticketId <= 0)
    {
        throw std::invalid_argument("Ticket ID must be greater than 0");
    }

    try
    {
        ticketService->cancelTicket(ticketId);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(
            "Failed to cancel ticket " + std::to_string(ticketId) + ": " + e.what());
    }
}

bool RMSFacade::getTrainAvailability(const int &trainId)
{
    auto train = trainService->getTrain(trainId);
    if (!train.has_value())
        throw std::out_of_range("train with id : " + std::to_string(trainId) + "does not exist");

    auto allocator = train->getSeatAllocator();
    if (!allocator) // null
        throw std::runtime_error(
            "Train with ID " + std::to_string(trainId) + " has no seat allocator");
    return allocator->hasAvailableSeats();
}

Train RMSFacade::updateTrain(int trainId, const std::string &name, int seats)
{
    // Validate inputs
    if (trainId <= 0)
    {
        throw std::invalid_argument("Train ID must be greater than 0");
    }
    if (name.empty() || name.find_first_not_of(" \t\n\r") == std::string::npos)
    {
        throw std::invalid_argument("Train name cannot be empty");
    }
    if (seats < 0)
    {
        throw std::invalid_argument("Seats cannot be negative");
    }
    auto currentTrain = trainService->getTrain(trainId);
    if (!currentTrain.has_value())
        throw std::out_of_range("train with id : " + std::to_string(trainId) + "does not exist");

    int currentSeats = currentTrain->getTotalSeats();
    // update train
    auto updatedTrain = trainService->updateTrain(trainId, name, seats);
    //    book tickets for the waiting list
    if (currentSeats < seats)
    { // new Seats added
        auto waitingListCopy = updatedTrain.getSeatAllocator()->getWaitingList();
        int seatsToAdd = seats - currentSeats;
        while (!waitingListCopy.empty() && seatsToAdd > 0)
        {
            int frontPassengerId = waitingListCopy.front();
            waitingListCopy.pop();
            try
            {
                ticketService->bookTicket(trainId, frontPassengerId);
                seatsToAdd--;
            }
            catch (const std::exception &e)
            {
                // log error and continue to the  remaining passengers
                std::cerr << "Failed to book ticket for waiting passenger "
                          << frontPassengerId << ": " << e.what() << "\n";
            }
        }
        // Save updated train with new seat allocation
        try
        {
            trainService->save(updatedTrain);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(
                "Failed to save updated train: " + std::string(e.what()));
        }
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
