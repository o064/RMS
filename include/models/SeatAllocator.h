//
// Created by Omar on 11/19/2025.
//


#ifndef RMS_SEATALLOCATOR_H
#define RMS_SEATALLOCATOR_H

#include <map>
#include <vector>
#include <queue>
#include <stack>

class SeatAllocator{
    std::priority_queue<int, std::vector<int>, std::greater<int> > availableSeats;
    std::queue<int> waitingList;
    std::map<int,int> allocatedSeats;
    std::stack<int> cancelledTickets;
    int totalSeats ;
public:
    SeatAllocator(int totalSeats);
    void freeSeat(int);

    int allocateSeat(int);
    int getAvailableSeatCount() const;
    bool hasAvailableSeat();

};
#endif //RMS_SEATALLOCATOR_H
