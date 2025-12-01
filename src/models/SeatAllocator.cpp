//
// Created by Omar on 11/20/2025.
//

#include "models/SeatAllocator.h"
#include <iostream>

SeatAllocator::SeatAllocator(const int& totalSeats){
    if(totalSeats <= 0 )
        this->totalSeats = 10 ;
    else
        this->totalSeats = totalSeats;
//    add the seats to the minHeap
    for(int seat = 1 ; seat <= this->totalSeats; seat++ )
        availableSeats.insert(seat);
}

int SeatAllocator::getAvailableSeatCount() const {
//    return the numbers of seats in the heap and the cancelled seats
    return availableSeats.size() + cancelledSeats.size() ;
}



int SeatAllocator::allocateSeat(const int& passengerId) {
    // Check for duplicate passenger ID

//    check in assigned seats
    for (auto it = allocatedSeats.begin(); it != allocatedSeats.end(); it++){
        if(it->second == passengerId){
            std::cout << "Passenger ID " << passengerId << " is already allocated a seat.\n";
            return -1;
        }
    }
    //cheeck if in waiting list
    std::queue<int> tempQueue = waitingList;
    while(!tempQueue.empty()){
        if(tempQueue.front() == passengerId){
            std::cout << "the passenger is already in the waiting list";
            return -1 ;
        }
        tempQueue.pop();
    }

    // no avaiable seat so add to the waiting list
    if(!hasAvailableSeats()){
        waitingList.push(passengerId);
        std::cout << "Train full, passenger added to waiting list\n";
        return -1;
    }
    // assign seat
    int seatNumber ;
    if(!cancelledSeats.empty()){
        seatNumber = cancelledSeats.top();
        cancelledSeats.pop();
    }
    else{
        auto minAvailbleSeat = availableSeats.begin();
        seatNumber = *minAvailbleSeat;
        availableSeats.erase(minAvailbleSeat);

    }
    allocatedSeats[seatNumber] = passengerId;
    return seatNumber;
}

int SeatAllocator::freeSeat(const int& seatNumber) {
    if(totalSeats  <  seatNumber  || seatNumber <= 0 ){
        std::cout << "this seat not exist in the train \n" ;
        return -1 ;
    }
//    if seat is not in allocated seats say it already not assigned to anyone
    const auto seat = allocatedSeats.find(seatNumber);
    if(seat == allocatedSeats.end() || seat->second == -1 ){ // not found or cancelled but not allocated
        std::cout << "this seat not yet allocated \n" ;
        return -1;
    }
    allocatedSeats[seat->first] = -1; // mark as unAllocated
    cancelledSeats.push(seat->first); // push to the stack
// if there is someone in waiting list assign the seat it him\her
    if(!waitingList.empty()){// waiting list is not empty
        const int firstPassengerId = waitingList.front();
        waitingList.pop();
        // push seat to cancelled stack for later allocation
        std::cout << "Seat " << seatNumber
                  << " freed and waiting passenger ID " << firstPassengerId
                  << " can now book it\n";
        return firstPassengerId;
    }
    // if there is no one in waiting list return 0
    return 0 ;
}


bool SeatAllocator::hasAvailableSeats() const {
    // if the is seat in cancelled tickets or minHeap
//   return avaiable(true) if availableSeats or cancelledSeats is not  empty
    return !availableSeats.empty() || !cancelledSeats.empty();
}

std::unique_ptr<SeatAllocator> SeatAllocator::clone() const  {
    return std::make_unique<SeatAllocator>(*this);
}

SeatAllocator::SeatAllocator(const SeatAllocator& other)
        : totalSeats(other.totalSeats),
          availableSeats(other.availableSeats),
          waitingList(other.waitingList),
          allocatedSeats(other.allocatedSeats),
          cancelledSeats(other.cancelledSeats) {
}

SeatAllocator& SeatAllocator::operator=(const SeatAllocator& other) {
    if (this != &other) {
        totalSeats = other.totalSeats;
        availableSeats = other.availableSeats;
        waitingList = other.waitingList;
        allocatedSeats = other.allocatedSeats;
        cancelledSeats = other.cancelledSeats;
    }
    return *this;
}

void SeatAllocator::addSeats(const int seats) {
    if(seats <= 0 )
        throw std::runtime_error("Seats must be greater than zero");

    int prevTotalSeats =totalSeats;
    totalSeats = prevTotalSeats + seats; // update seats
    for(int i = prevTotalSeats + 1 ; i <= totalSeats ; i++)
        availableSeats.insert(i); // update heap


}

void SeatAllocator::changeTotalSeats(const int newTotalSeats) {
    // 1) newTotalSeats can be larger than the current seat , so expand ( 30 | 20 )
    // 2) newTotalSeats can be smaller than but there is space to shrink  (  | 20 , 18 )
//    3) smaller + no space to shrink

    if(newTotalSeats < getAllocatedSeatCount()) // smaller than allocated seats
        throw std::runtime_error("Cannot reduce total seats because some of the higher seats are allocated");
    if(newTotalSeats < totalSeats){ // shrink

        int seatsToRemove  = totalSeats - newTotalSeats;
        if (availableSeats.size() < seatsToRemove)
            throw std::runtime_error("Cannot shrink: not enough free seats");


        while (seatsToRemove > 0 ){
            auto lastSeat = std::prev(availableSeats.end()); // end point to end of set not the last element
            availableSeats.erase(lastSeat);
            seatsToRemove--;
        }


    }else if (newTotalSeats > totalSeats) // expand
        for(int i = totalSeats + 1 ; i  <= newTotalSeats ; i++)
            availableSeats.insert(i);

    totalSeats = newTotalSeats;

}

int SeatAllocator::getAllocatedSeatCount() const {
    return totalSeats - getAvailableSeatCount();
}

std::queue<int> SeatAllocator::getWaitingList() const {
    return waitingList;
}

int SeatAllocator::getTotalSeats() const {
    return totalSeats;
}
