//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_PASSENGERSERVICE_H
#define RMS_PASSENGERSERVICE_H

#include <vector>
#include "../models/Passenger.h"
#include "../Repo/IPassengerRepository.h"

class PassengerService {
private:
    IPassengerRepository* passengerRepository;
public:
    PassengerService(IPassengerRepository* repo);
    Passenger* getPassenger(int passengerId);
    std::vector<Passenger> getAllPassengers();
    Passenger createPassenger(std::string name);
    Passenger deletePassenger(int passengerId);
    Passenger* findPassenger(std::string name);
};
#endif //RMS_PASSENGERSERVICE_H
