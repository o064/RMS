//
// Created by Omar on 11/24/2025.
//
#include <stdexcept>
#include "Services/PassengerService.h"
#include "utils/helpers.h"
PassengerService::PassengerService(IPassengerRepository *repo) {
    this->passengerRepository =repo;
}

Passenger PassengerService::getPassenger(const int &passengerId) {
    auto p = passengerRepository->getPassenger(passengerId);
    if(!p.has_value())
        throw std::out_of_range("Passenger with id " + std::to_string(passengerId) + " does not exist");

    return  p.value();
}

vector<Passenger> PassengerService::getAllPassengers() {
    return passengerRepository->getAllPassengers();
}

void PassengerService::deletePassenger(const int &passengerId) {
    bool deleted = passengerRepository->deletePassenger(passengerId);
    if (!deleted)
        throw std::out_of_range("failed to delete passenger with id : " + std::to_string(passengerId));
}

Passenger PassengerService::createPassenger(const std::string& name) {
    Passenger p(0,trim(name));
    passengerRepository->save(p);
    return p;
}
Passenger PassengerService::updatePassenger(const int passengerId , const std::string& name) {
    auto passenger = this->getPassenger(passengerId);
    passenger.setName(name); //update name
    passengerRepository->save(passenger);
    return passenger;
}

Passenger PassengerService::find_or_create_passenger(const std::string &name) {
    const vector<Passenger> passengers = passengerRepository->getAllPassengers();
    // search if it is existed
    for(const auto & p : passengers)
        if(toLowerCase(p.getName()) == toLowerCase(name))
            return   p;
    //else  create passenger
    Passenger p(0,name);
    passengerRepository->save(p);
    return p;
}

