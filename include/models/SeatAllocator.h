//
// Created by Omar on 11/19/2025.
//


#ifndef RMS_SEATALLOCATOR_H
#define RMS_SEATALLOCATOR_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
#include "../structures/stack.h"
#include <string>
#include<memory>

class SeatAllocator{
    std::set<int> availableSeats;
    std::queue<int> waitingList;
    std::unordered_map<int, int> allocatedSeats;
    stack<int> cancelledSeats;
    int totalSeats ;
public:

    SeatAllocator(const int& totalSeats = 10);
    // for copying
    std::unique_ptr<SeatAllocator> clone() const;
    SeatAllocator(const SeatAllocator& other);
    SeatAllocator& operator=(const SeatAllocator& other);

    void addSeats(const int seats);
    void changeTotalSeats(const int newTotalSeats);

    int freeSeat(const int& seatNumber);
    int allocateSeat(const int& passengerId);

    int getAvailableSeatCount() const;
    int getAllocatedSeatCount() const;
    int getTotalSeats() const;

    std::queue<int> getWaitingList()const;

    bool hasAvailableSeats() const;

};
#endif //RMS_SEATALLOCATOR_H
