//
// Created by Omar on 11/21/2025.
//

#include "Services/TrainService.h"
#include <stdexcept> //for run time exception

std::optional<Train> TrainService::getTrain(const int& trainId) {
    return trainRepository->getTrainById(trainId);
}

TrainService::TrainService(ITrainRepository *repo) {
    this->trainRepository = repo;
}

TrainService::~TrainService() {
    // do not delete repo because service is not owning the  repo it just use it
}

std::vector<Train> TrainService::getAllTrains() {
    return trainRepository->getAllTrains();
}

Train TrainService::createTrain(const std::string& name,const int& seats) {
    Train t(0,name ,seats);
    trainRepository->save(t); // save the train  and give id by the repo
    return t;
}

bool TrainService::deleteTrain(int trainId) {
    return trainRepository->deleteTrain(trainId);
}

bool TrainService::isAvailbleSeat(int trainId) {
    auto train = trainRepository->getTrainById(trainId);
    if(!train.has_value()){
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");
    }
    return train->hasAvailableSeats() ;
}

void TrainService::save(Train &train) {
    trainRepository->save(train);
}
