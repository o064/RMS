//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_IPASSENGERREPOSITORY_H
#define RMS_IPASSENGERREPOSITORY_H

#include <vector>
#include "../models/Passenger.h"

class IPassengerRepository {
public:
    virtual Passenger* getPassenger(int passengerId) = 0;
    virtual bool deletePassenger(int passengerId) = 0;
    virtual Passenger save(Passenger passenger) = 0;
    virtual std::vector<Passenger> getAllPassengers() = 0;
    virtual  void clear()   =  0;

    virtual ~IPassengerRepository() = default;
};
#endif //RMS_IPASSENGERREPOSITORY_H
