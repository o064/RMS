#include "models/Train.h"
#include <iostream>

Train::Train(int id, const std::string& name, int totalSeats)
        : id(id), name(name), totalSeats(totalSeats),
          seatAllocator(std::make_unique<SeatAllocator>(totalSeats)) {
}

int Train::getTrainId() const {
    return id;
}

std::string Train::getTrainName() const {
    return name;
}

void Train::setTrainName(const std::string& name) {
    this->name = name;
}

void Train::setTrainId(int trainId) {
    this->id = trainId;
}

SeatAllocator* Train::getSeatAllocator() const {
    return seatAllocator.get();
}

bool Train::hasAvailableSeats() const {
    return seatAllocator->hasAvailableSeats();
}

int Train::getTotalSeats() const {
    return totalSeats;
}