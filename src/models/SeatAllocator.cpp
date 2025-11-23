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
        availableSeats.push(seat);
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
    auto tempQueue = waitingList;
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
        seatNumber = availableSeats.top();
        availableSeats.pop();

    }
    allocatedSeats[seatNumber] = passengerId;
    return seatNumber;
}

void SeatAllocator::freeSeat(const int& seatNumber) {
    if(totalSeats  <  seatNumber  || seatNumber <= 0 ){
        std::cout << "this seat not exist in the train \n" ;
        return;
    }
//    if seat is not in allocated seats say it already not assigned to anyone
    const auto seat = allocatedSeats.find(seatNumber);
    if(seat == allocatedSeats.end() || seat->second == -1 ){ // not found or cancelled but not allocated
        std::cout << "this seat not yet allocated \n" ;
        return;
    }
// if there is someOne in waiting list assign the seat it him\her
    if(!waitingList.empty()){// waiting list is not empty
        const int firstPassengerId = waitingList.front();
        waitingList.pop();
        allocatedSeats[seat->first] = firstPassengerId;
        std::cout << "seat " << seat->first
        << " is assigned to passenger with id :"
        << firstPassengerId << "from the waitining list " << std::endl;
        return ;
    }
    // if there is no one in waiting list add to the stack for the next passenger
    allocatedSeats[seat->first] = -1; // mark as unAllocated
    cancelledSeats.push(seat->first); // push to the stack
}

void SeatAllocator::printStatus() const {
        std::cout << "\n=== Seat Allocator Status ===" << std::endl;
        std::cout << "Total Seats: " << totalSeats << std::endl;
        std::cout << "Available Seats Count: " << getAvailableSeatCount() << std::endl;
        // Print available seats (need to copy since priority_queue doesn't allow iteration)
        std::priority_queue<int, std::vector<int>, std::greater<int>> availableCopy = availableSeats;
        std::cout << "Available Seats: ";
        if (availableCopy.empty()) {
            std::cout << "None";
        } else {
            while (!availableCopy.empty()) {
                std::cout << availableCopy.top() << " ";
                availableCopy.pop();
            }
        }
        std::cout << std::endl;

        // Print waiting list
        std::queue<int> waitingCopy = waitingList;
        std::cout << "Waiting List: ";
        if (waitingCopy.empty()) {
            std::cout << "None";
        } else {
            while (!waitingCopy.empty()) {
                std::cout << waitingCopy.front() << " ";
                waitingCopy.pop();
            }
        }
        std::cout << std::endl;

        // Print allocated seats
        std::cout << "Allocated Seats: ";
        if (allocatedSeats.empty()) {
            std::cout << "None";
        } else {
            for (const auto& pair : allocatedSeats) {
                std::cout << "Seat " << pair.first << "->Passenger " << pair.second << ", ";
            }
        }
        std::cout << std::endl;

        // Print cancelled seats
        std::stack<int> cancelledCopy = cancelledSeats;
        std::cout << "Cancelled Seats Stack: ";
        if (cancelledCopy.empty()) {
            std::cout << "None";
        } else {
            while (!cancelledCopy.empty()) {
                std::cout << cancelledCopy.top() << " ";
                cancelledCopy.pop();
            }
        }
        std::cout << std::endl;

        std::cout << "Has Available Seats: " << (hasAvailableSeats()? "Yes" : "No") << std::endl;
        std::cout << "=============================\n" << std::endl;

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