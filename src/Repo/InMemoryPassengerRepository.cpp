//
// Created by Omar on 11/24/2025.
//

#include <stdexcept>
#include <iostream>
#include "Repo/InMemoryPassengerRepository.h"

Passenger InMemoryPassengerRepository::getPassenger(const int &passengerId) {
    auto  result = passengers.find(passengerId);
    if(result != passengers.end()){
        return result->second;
    }
    throw std::runtime_error("Passenger not found");
}

std::list<Passenger> InMemoryPassengerRepository::getAllPassengers() {
    std::list<Passenger> results ;
    for(const auto & ps : passengers){
        results.push_back(ps.second);
    }
    return results;
}

void InMemoryPassengerRepository::save(Passenger &passenger) {
    if(passenger.getId() == 0 ){
        passenger.setId(next_id++);
    }else if(passenger.getId() >= next_id){
        next_id =passenger.getId() + 1;
    }
    const int id =passenger.getId();
    auto res = passengers.emplace(id,passenger);
    if(!res.second){ //update
        res.first->second = passenger;
    }


}

bool InMemoryPassengerRepository::deletePassenger(const int &passengerId) {
    auto it = passengers.find(passengerId);
    if (it != passengers.end()) {
        passengers.erase(it);
        return true;
    }
    return false;
}

void InMemoryPassengerRepository::clear() {
    passengers.clear();
    next_id= 1;
    std::cout << "All passengers destroyed\n";
}

