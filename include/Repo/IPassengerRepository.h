//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_IPASSENGERREPOSITORY_H
#define RMS_IPASSENGERREPOSITORY_H

#include <vector>
#include "../models/Passenger.h"
#include <memory>

class IPassengerRepository
{
public:
    virtual std::shared_ptr<Passenger> getPassenger(int passengerId) = 0;
    virtual bool deletePassenger(int passengerId) = 0;
    virtual Passenger save(Passenger passenger) = 0;
    virtual std::vector<std::shared_ptr<Passenger>> getAllPassengers() = 0;
    virtual void clear() = 0;

    virtual ~IPassengerRepository() = default;
};
#endif // RMS_IPASSENGERREPOSITORY_H
