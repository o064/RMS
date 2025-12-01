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
    //crud
    std::optional<Train> getTrain(const int&);
    std::vector<Train> getAllTrains();
    Train createTrain(const std::string& name,int seats);
    Train updateTrain(const int& id , const std::string& name,int seats = 0);
    bool deleteTrain(int trainId);

    //seats
    Train addSeats(const int trainId , const int seats);
    Train addSeats(const std::string name  , const int seats);

    bool isAvailbleSeat(int trainId);
    void save(Train & train);
};
#endif //RMS_TRAINSERVICE_H
