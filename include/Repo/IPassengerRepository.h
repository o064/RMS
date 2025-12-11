//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_IPASSENGERREPOSITORY_H
#define RMS_IPASSENGERREPOSITORY_H

#include "../structures/vector.h"
#include "../models/Passenger.h"
#include <optional>

class IPassengerRepository
{
public:
    virtual std::optional<Passenger> getPassenger(const int& passengerId) = 0;
    virtual bool deletePassenger(const int& passengerId) = 0;
    virtual void save( Passenger& passenger) = 0;
    virtual vector<Passenger> getAllPassengers() = 0;
    virtual void clear() = 0;

    virtual ~IPassengerRepository() = default;
};
#endif // RMS_IPASSENGERREPOSITORY_H
