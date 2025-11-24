//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_TRAINSERVICE_H
#define RMS_TRAINSERVICE_H

#include "../Repo/ITrainRepository.h"
#include <optional>

class TrainService{
private:
    ITrainRepository* trainRepository;
public:
    TrainService(ITrainRepository* repo) ;
    ~TrainService();

    std::optional<Train> getTrain(const int&);
    std::list<Train> getAllTrains();
    Train createTrain(const std::string& name,const int& seats);
    bool deleteTrain(int trainId);
    bool isAvailbleSeat(int trainId);
    void save(Train & train);
};
#endif //RMS_TRAINSERVICE_H
