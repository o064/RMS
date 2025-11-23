#include "models/Train.h"
#include <iostream>

Train::Train(const int& id, const std::string& name, const int& totalSeats )
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

std::unique_ptr<Train> Train::clone() const {

        auto newTrain = std::make_unique<Train>(id, name, totalSeats);
        if (seatAllocator) {
            newTrain->seatAllocator = seatAllocator->clone();
        }
        return newTrain;

}

Train::Train(const Train &other)
        : id(other.id), name(other.name), totalSeats(other.totalSeats),
          seatAllocator(other.seatAllocator ? other.seatAllocator->clone() : nullptr) {}

Train &Train::operator=(const Train &other) {

        if (this != &other) {
            id = other.id;
            name = other.name;
            totalSeats = other.totalSeats;
            seatAllocator = other.seatAllocator ? other.seatAllocator->clone() : nullptr;
        }
        return *this;

}
