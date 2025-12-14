//
// Created by Omar on 11/19/2025.
//


#ifndef RMS_SEATALLOCATOR_H
#define RMS_SEATALLOCATOR_H

#include <set>
#include "../structures/stack.h"
#include "../structures/queue.h"
#include "../structures/unordered_map.h"
#include <string>
#include<memory>
#include <functional>

class SeatAllocator{
    std::set<int> availableSeats;
    queue<int> waitingList;
    std::set<int> waitingSet;              // prevent duplicate waiting entries
    unordered_map<int, int> allocatedSeats;
    stack<int> cancelledSeats;
    int totalSeats ;
public:

    SeatAllocator( int totalSeats = 10);
    // for copying
    std::unique_ptr<SeatAllocator> clone() const;
    SeatAllocator(const SeatAllocator& other);
    SeatAllocator& operator=(const SeatAllocator& other);

    void addSeats(int seats);
    void changeTotalSeats(int newTotalSeats);

    int freeSeat( int seatNumber);
    int allocateSeat( int passengerId);
    int processWaitingList(int seatsToAdd, std::function<void(int)> bookCallback)  ;


    int getAvailableSeatCount() const;
    int getAllocatedSeatCount() const;
    int getTotalSeats() const;
    int getWaitingListSize()const;

    queue<int> getWaitingList()const;

    bool hasAvailableSeats() const;

    void printStatus() const;

};
#endif //RMS_SEATALLOCATOR_H
