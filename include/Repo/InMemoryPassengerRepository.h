//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYPASSENGERREPOSITORY_H
#define RMS_INMEMORYPASSENGERREPOSITORY_H

#include <vector>
#include <map>
#include "../models/Passenger.h"
#include "IPassengerRepository.h"
#include <memory>

class InMemoryPassengerRepository : public IPassengerRepository
{
private:
    std::map<int, std::shared_ptr<Passenger>> passengers;

public:
    std::shared_ptr<Passenger> getPassenger(int passengerId) override;
    bool deletePassenger(int passengerId) override;
    Passenger save(Passenger passenger) override;
    std::vector<std::shared_ptr<Passenger>> getAllPassengers() override;
    void clear();
};
#endif // RMS_INMEMORYPASSENGERREPOSITORY_H
