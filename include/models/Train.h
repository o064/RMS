//
// Created by Omar on 11/18/2025.
//



#ifndef RMS_TRAIN_H
#define RMS_TRAIN_H

#include "SeatAllocator.h"
#include <string>

class Train {
private:
    int id;
    std::string name ;
    int totalSeats ;
    SeatAllocator* seatAllocator;
public:
    Train(int id , std::string name , int totalSeats);

    int getTrainId() const ;
    bool hasAvailbeSeates() ;
    SeatAllocator getSeatAllocator() const ;
    void setSeatAllocator(SeatAllocator);
};


#endif //RMS_TRAIN_H
