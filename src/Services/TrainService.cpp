//
// Created by Omar on 11/21/2025.
//

#include "Services/TrainService.h"

Train TrainService::getTrain(const int& trainId) {
    return trainRepository->getTrainById(trainId);
}

TrainService::TrainService(ITrainRepository *repo) {
    this->trainRepository = repo;
}

TrainService::~TrainService() {
    // do not delete repo because service is not owning the  repo it just use it
}

std::list<Train> TrainService::getAllTrains() {
    return trainRepository->getAllTrains();
}

Train TrainService::createTrain(const std::string& name,const int& seats) {
    Train t(0,name ,seats);
    return trainRepository->save(t);
}

bool TrainService::deleteTrain(int trainId) {
    return trainRepository->deleteTrain(trainId);
}

bool TrainService::isAvailbleSeat(int trainId) {
    return  trainRepository->getTrainById(trainId).hasAvailableSeats();
}
