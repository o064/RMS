//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYPASSENGERREPOSITORY_H
#define RMS_INMEMORYPASSENGERREPOSITORY_H

#include <vector>
#include <map>
#include "../models/Passenger.h"
#include "IPassengerRepository.h"

class InMemoryPassengerRepository : public IPassengerRepository {
private:
    std::map<int, Passenger> passengers;
public:
    Passenger* getPassenger(int passengerId) override;
    bool deletePassenger(int passengerId) override;
    Passenger save(Passenger passenger) override;
    std::vector<Passenger> getAllPassengers() override;
    void clear();
};
#endif //RMS_INMEMORYPASSENGERREPOSITORY_H
