//
// Created by Omar on 11/24/2025.
//
#include "Services/PassengerService.h"
#include "utils/helpers.h"
PassengerService::PassengerService(IPassengerRepository *repo) {
    this->passengerRepository =repo;
}

std::optional<Passenger> PassengerService::getPassenger(const int &passengerId) {
    return passengerRepository->getPassenger(passengerId);
}

std::list<Passenger> PassengerService::getAllPassengers() {
    return passengerRepository->getAllPassengers();
}

bool PassengerService::deletePassenger(const int &passengerId) {
    return passengerRepository->deletePassenger(passengerId);
}

Passenger PassengerService::createPassenger(const std::string& name) {
    Passenger p(0,name);
    passengerRepository->save(p);
    return p;
}

std::optional<Passenger> PassengerService::find_or_create_passenger(const std::string &name) {
    const std::list<Passenger> passengers = passengerRepository->getAllPassengers();

    for(const auto & p : passengers)
        if(toLowerCase(p.getName()) == toLowerCase(name))
            return passengerRepository->getPassenger(p.getId());
    Passenger p(0,name);
    passengerRepository->save(p);
    return p;
}

