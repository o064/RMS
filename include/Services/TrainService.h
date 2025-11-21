//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TRAINSERVICE_H
#define RMS_TRAINSERVICE_H

#include "../Repo/ITrainRepository.h"

class TrainService{
private:
    ITrainRepository* trainRepository;
public:
    TrainService(ITrainRepository* repo) ;
    ~TrainService();

    std::shared_ptr<Train> getTrain(int);
    std::vector<std::shared_ptr<Train>> getAllTrains();
    std::shared_ptr<Train> createTrain(std::string name, int seats);
    bool deleteTrain(int trainId);
    bool isAvailbleSeat(int trainId);
};
#endif //RMS_TRAINSERVICE_H
