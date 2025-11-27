//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYPASSENGERREPOSITORY_H
#define RMS_INMEMORYPASSENGERREPOSITORY_H

#include <vector>
#include <map>
#include <optional>

#include "../models/Passenger.h"
#include "IPassengerRepository.h"

class InMemoryPassengerRepository : public IPassengerRepository
{
private:
    std::map<int, Passenger> passengers;
    int next_id = 1;
public:
    std::optional<Passenger> getPassenger(const int& passengerId) override;
    bool deletePassenger(const int& passengerId) override;
    void save( Passenger& passenger) override;
    std::vector<Passenger> getAllPassengers() override;
    void clear() override;
};
#endif // RMS_INMEMORYPASSENGERREPOSITORY_H
