//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYPASSENGERREPOSITORY_H
#define RMS_INMEMORYPASSENGERREPOSITORY_H

#include "../structures/vector.h"
#include <optional>

#include "../models/Passenger.h"
#include "../structures/map.h"
#include "IPassengerRepository.h"

class InMemoryPassengerRepository : public IPassengerRepository
{
private:
    Map<int, Passenger> passengers;
    int next_id = 1;
public:
    std::optional<Passenger> getPassenger(const int& passengerId) override;
    bool deletePassenger(const int& passengerId) override;
    void save( Passenger& passenger) override;
    vector<Passenger> getAllPassengers() override;
    void clear() override;
};
#endif // RMS_INMEMORYPASSENGERREPOSITORY_H
