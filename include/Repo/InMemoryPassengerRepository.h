//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYPASSENGERREPOSITORY_H
#define RMS_INMEMORYPASSENGERREPOSITORY_H

#include <list>
#include <map>
#include "../models/Passenger.h"
#include "IPassengerRepository.h"

class InMemoryPassengerRepository : public IPassengerRepository
{
private:
    std::map<int, Passenger> passengers;
    int next_id = 1;
public:
    Passenger getPassenger(const int& passengerId) override;
    bool deletePassenger(const int& passengerId) override;
    Passenger save( Passenger& passenger) override;
    std::list<Passenger> getAllPassengers() override;
    void clear() override;
};
#endif // RMS_INMEMORYPASSENGERREPOSITORY_H
