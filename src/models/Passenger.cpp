//
// Created by Omar on 11/19/2025.
//
#include <stdexcept>
#include "models/Passenger.h"





Passenger::Passenger(const int id, const std::string &name) {
    if(name.empty()) throw std::runtime_error("Invalid input name");
    if(id <0) throw std::runtime_error("Invalid id");
    this->id = id;
    this->name =name;
}

int Passenger::getId() const {
    return this->id;
}

std::string Passenger::getName() const {
    return this->name;
}

void Passenger::setId(const int &passengerId) {
    if(passengerId <=0) throw std::runtime_error("Invalid id");
    this->id = passengerId;
}

void Passenger::setName(const std::string& name) {
    if(name.empty()) throw std::runtime_error("Invalid input name");
    this->name = name;
}



