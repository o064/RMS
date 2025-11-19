//
// Created by Omar on 11/19/2025.
//


#ifndef RMS_ITRAINREPOSATORY_H
#define RMS_ITRAINREPOSATORY_H

#include "../models/Train.h"
#include <vector>

class ITrainRepository{
public:
    virtual  std::vector<Train> getAllTrains() =  0;
    virtual  bool deleteTrain(int) =  0;
    virtual  Train save(Train) =  0;
    virtual  Train* getTrainById(int) =  0;
    virtual ~ITrainRepository() = default;


};
#endif //RMS_ITRAINREPOSATORY_H
