//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_PASSENGERSERVICE_H
#define RMS_PASSENGERSERVICE_H

#include "../structures/vector.h"
#include "../models/Passenger.h"
#include "../Repo/IPassengerRepository.h"

class PassengerService
{
private:
    IPassengerRepository *passengerRepository;

public:
    PassengerService(IPassengerRepository *repo);
    std::optional<Passenger> getPassenger(const int& passengerId);
    vector<Passenger> getAllPassengers();
    Passenger createPassenger(const std::string& name);
    Passenger updatePassenger(const int passengerId , const std::string& name);
    bool deletePassenger(const int& passengerId);
    Passenger find_or_create_passenger(const std::string& name);
};
#endif // RMS_PASSENGERSERVICE_H
