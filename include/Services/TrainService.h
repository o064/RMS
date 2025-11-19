//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TRAINSERVICE_H
#define RMS_TRAINSERVICE_H

#include "../Repo/ITrainReposatory.h"

class TrainService{
private:
    ITrainRepository* trainRepository;
public:
    TrainService(ITrainRepository* repo) ;

    Train* getTrain(int);
    std::vector<Train> getAllTrains();
    Train createTrain(std::string name, int seats);
    bool deleteTrain(int);
    bool isAvailbleSeat();
};
#endif //RMS_TRAINSERVICE_H
