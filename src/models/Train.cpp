#include "models/Train.h"
#include "utils/helpers.h"

#include <iostream>
using std::cout;
using std::endl;

Train::Train(const int id, const std::string& name, const int totalSeats ){
    if(id < 0 ) throw std::invalid_argument("Invalid  negative id");
    if(!isValidName(name)) throw std::invalid_argument("Invalid input name");
    if(totalSeats < 0) throw std::invalid_argument("total seat must be greater than 0");
    this->id = id;
    this->name = trim(name);
    this->totalSeats = totalSeats;
    this->seatAllocator = std::make_unique<SeatAllocator>(totalSeats);

}

int Train::getTrainId() const {
    return id;
}

std::string Train::getTrainName() const {
    return name;
}

void Train::setTrainName(const std::string& name) {
    if(!isValidName(name) )
        throw std::invalid_argument("Invalid input name");
    this->name = trim(name);
}

void Train::setTrainId(int trainId) {
    if(trainId <= 0 ) throw std::runtime_error("Invalid id");
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

void Train::addSeats(int seats) {
    if(seats <= 0 )
        throw std::invalid_argument("Seats must be greater than zero");
    seatAllocator->addSeats(seats);

    this->totalSeats = seatAllocator->getTotalSeats();
}

void Train::setSeats(int seats) {
    if(seats <= 0)
        throw std::invalid_argument("Seats must be greater than zero");
    seatAllocator->changeTotalSeats(seats);

    totalSeats =seatAllocator->getTotalSeats();
}

void Train::trainStatus()const {
    if (!seatAllocator) {
        cout << "Train " << name << " has no seat allocator initialized.\n";
        return;
    }

    int waitingSize = seatAllocator->getWaitingListSize();
    cout << "\n=========== Train Status ===========\n";
    cout << "Train ID      : " << id << "\n";
    cout << "Train Name    : " << name << "\n";
    cout << "Total Seats   : " << totalSeats << "\n";
    cout << "Waiting List  : " << waitingSize << "\n";
    cout << "====================================\n\n";

    seatAllocator->printStatus();

}

void Train::print(const std::string& msg) const{
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Train ID   : " << id<< "\n";
    cout << "Name   : " << name << "\n";
    cout << "Seats  : " << totalSeats << "\n";
    cout << "Availability: " << (hasAvailableSeats() ? "Available" : "full") << "\n";
    cout << "--------------------------------------------------\n";
}

