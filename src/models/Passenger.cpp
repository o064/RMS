//
// Created by Omar on 11/19/2025.
//

#include "models/Passenger.h"
Passenger::Passenger(int id, std::string name)
{
    this->id = id;
    this->name = name;
}

int Passenger::getId() const
{
    return id;
}

std::string Passenger::getName() const
{
    return name;
}