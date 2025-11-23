//
// Created by Omar on 11/19/2025.
//


#ifndef RMS_SEATALLOCATOR_H
#define RMS_SEATALLOCATOR_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include<memory>

class SeatAllocator{
    std::priority_queue<int, std::vector<int>, std::greater<int> > availableSeats;
    std::queue<int> waitingList;
    std::unordered_map<int, int> allocatedSeats;
    std::stack<int> cancelledSeats;
    int totalSeats ;
public:

    SeatAllocator(const int& totalSeats = 10);
    // for copying
    std::unique_ptr<SeatAllocator> clone() const;
    SeatAllocator(const SeatAllocator& other);
    SeatAllocator& operator=(const SeatAllocator& other);

    void freeSeat(const int& seatNumber);
    int allocateSeat(const int& passengerId);
    int getAvailableSeatCount() const;
    bool hasAvailableSeats() const;
    void printStatus() const ;

};
#endif //RMS_SEATALLOCATOR_H
