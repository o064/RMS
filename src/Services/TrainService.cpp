//
// Created by Omar on 11/21/2025.
//

#include "Services/TrainService.h"
#include "utils/helpers.h"
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

Train TrainService::createTrain(const std::string& name,int seats) {
    Train t(0,name ,seats);
    trainRepository->save(t); // save the train  and give id by the repo
    return t;
}

bool TrainService::deleteTrain(int trainId) {
    return trainRepository->deleteTrain(trainId);
}

bool TrainService::isAvailbleSeat(int trainId) {
    auto train = trainRepository->getTrainById(trainId);
    if(!train.has_value())
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");

    return train->hasAvailableSeats() ;
}

void TrainService::save(Train &train) {
    trainRepository->save(train);
}

Train TrainService::updateTrain(const int &trainId, const std::string &name, int seats) {
    auto train = trainRepository->getTrainById(trainId);
    if(!train.has_value())  // not exist
        throw std::runtime_error("cannot update train because train with id : " +  std::to_string(trainId) + "does not exit") ;
    // update name
    if(!name.empty())
        train->setTrainName(name);
    // update seats
    if(seats != 0)
        train->setSeats(seats);
    trainRepository->save(train.value());
    return train.value();
}

Train TrainService::addSeats(const int trainId, const int seats) {
    auto train =trainRepository->getTrainById(trainId);
    if(!train.has_value())
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");
    train->addSeats(seats);
    trainRepository->save(train.value());
    return train.value();
}

Train TrainService::addSeats(const std::string name, const int seats) {
    if(name.empty())
        throw std::runtime_error("invalid name (empty string)");

    auto trains = trainRepository->getAllTrains();
    if(trains.empty())
        throw std::runtime_error("train with name : " + name +" does not exit");
    for(auto &train : trains){ // search train
        // if match update
        if(compareString(train.getTrainName() ,name))
            return  addSeats(train.getTrainId(),seats);
    }
    throw std::runtime_error("train with name : " + name +" does not exit");

}
