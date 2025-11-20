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
    Train(const int& id , const std::string& name , const int& totalSeats = 10);
    ~Train();
    int getTrainId() const ;
    std::string getTrainName() const ;
    void setTrainName(const std::string& name);
    bool hasAvailbeSeats() ;
    SeatAllocator* getSeatAllocator() const ;
//    void setSeatAllocator(SeatAllocator);
};


#endif //RMS_TRAIN_H
