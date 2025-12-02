//
// Created by Omar on 11/19/2025.
//
#include <stdexcept>
#include "models/Passenger.h"
#include "utils/helpers.h"

#include <iostream>
using std::cout;
using std::endl;




Passenger::Passenger(const int id, const std::string &name) {
    if(!isValidName(name)) throw std::invalid_argument("Invalid input name");
    if(id <0) throw std::invalid_argument("Invalid id");
    this->id = id;
    this->name = trim(name);
}

int Passenger::getId() const {
    return this->id;
}

std::string Passenger::getName() const {
    return this->name;
}

void Passenger::setId(const int &passengerId) {
    if(passengerId <=0) throw std::invalid_argument("Invalid id");
    this->id = passengerId;
}

void Passenger::setName(const std::string& name) {
    if(!isValidName(name)) throw std::invalid_argument("Invalid input name");
    this->name = trim(name);
}

void Passenger::print(const std::string &msg) const {
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Passenger ID   : " << id << "\n";
    cout << "Name   : " << name << "\n";
    cout << "--------------------------------------------------\n";
}



