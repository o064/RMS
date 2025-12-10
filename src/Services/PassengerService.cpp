//
// Created by Omar on 11/24/2025.
//
#include <stdexcept>
#include "Services/PassengerService.h"
#include "utils/helpers.h"
PassengerService::PassengerService(IPassengerRepository *repo) {
    this->passengerRepository =repo;
}

std::optional<Passenger> PassengerService::getPassenger(const int &passengerId) {

    return  passengerRepository->getPassenger(passengerId);
}

std::vector<Passenger> PassengerService::getAllPassengers() {
    return passengerRepository->getAllPassengers();
}

bool PassengerService::deletePassenger(const int &passengerId) {
    return passengerRepository->deletePassenger(passengerId);
}

Passenger PassengerService::createPassenger(const std::string& name) {
    Passenger p(0,trim(name));
    passengerRepository->save(p);
    return p;
}
Passenger PassengerService::updatePassenger(const int passengerId , const std::string& name) {
    auto passenger = passengerRepository->getPassenger(passengerId);
    if(!passenger.has_value())
        throw std::out_of_range("passenger with id: "+ std::to_string(passengerId) + " does not exit \n");
    passenger->setName(name); //update name
    passengerRepository->save(passenger.value());
    return passenger.value();
}

Passenger PassengerService::find_or_create_passenger(const std::string &name) {
    const std::vector<Passenger> passengers = passengerRepository->getAllPassengers();

    for(const auto & p : passengers)
        if(toLowerCase(p.getName()) == toLowerCase(name)){
            return   passengerRepository->getPassenger(p.getId()).value();
        }
    Passenger p(0,name);
    passengerRepository->save(p);
    return p;
}

