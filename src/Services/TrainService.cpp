//
// Created by Omar on 11/21/2025.
//

#include "Services/TrainService.h"

std::shared_ptr<Train> TrainService::getTrain(int trainId) {
    return trainRepository->getTrainById(trainId);
}

TrainService::TrainService(ITrainRepository *repo) {
    this->trainRepository = repo;
}

TrainService::~TrainService() {
    // do not delete repo because service is not owning the  repo it just use it
}

std::vector<std::shared_ptr<Train>> TrainService::getAllTrains() {
    return trainRepository->getAllTrains();
}

std::shared_ptr<Train> TrainService::createTrain(std::string name, int seats) {
    std::shared_ptr<Train> t = std::make_shared<Train>(0,name ,seats);
    return trainRepository->save(t);
}

bool TrainService::deleteTrain(int trainId) {
    return trainRepository->deleteTrain(trainId);
}

bool TrainService::isAvailbleSeat(int trainId) {
    return  trainRepository->getTrainById(trainId)->hasAvailableSeats();
}
