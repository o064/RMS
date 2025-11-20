//
// Created by Omar on 11/20/2025.
//
#include "models/Train.h"


Train::Train(const int& id , const std::string& name , const int& totalSeats) {
    this->id =id;
    this->name= name;
    // cannot have less than or eq zero seat
    if(totalSeats <= 0 )
        this->totalSeats = 10 ;
    else
        this->totalSeats = totalSeats ;

    this->seatAllocator = new SeatAllocator(this->totalSeats);
}
Train::~Train(){
    delete seatAllocator;
}

int Train::getTrainId() const { return this->id;}
std::string Train::getTrainName() const { return this->name;}

bool Train::hasAvailbeSeats() {
    return  seatAllocator->hasAvailableSeats();
}

SeatAllocator* Train::getSeatAllocator() const {
    return seatAllocator;
}

void Train::setTrainName(const std::string& name) {
    this->name=name;
}

