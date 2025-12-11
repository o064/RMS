#include "Repo/InMemoryTrainRepository.h"
#include <iostream>
#include <stdexcept>

vector<Train> InMemoryTrainRepository::getAllTrains() const {
    vector<Train> result;
    for (const auto& train : trains) {
        result.push_back(train.second);
    }
    return result;
}

void InMemoryTrainRepository::save(Train & newTrain) {
    // assign id if needed
    if (newTrain.getTrainId() == 0) {
        newTrain.setTrainId(next_id++);
    }else if(newTrain.getTrainId() >= next_id){
        next_id = newTrain.getTrainId() + 1;
    }

    int trainId = newTrain.getTrainId();

    auto result = trains.emplace(trainId, newTrain);

    // If trainId already existed, update it
    if (!result.second) {
        result.first->second = newTrain;
    }

}

bool InMemoryTrainRepository::deleteTrain(int trainId) {
    auto it = trains.find(trainId);
    if (it != trains.end()) {
        trains.erase(it);
        return true;
    }
    return false;
}

std::optional<Train>   InMemoryTrainRepository::getTrainById(const int& trainId) const {
    auto it = trains.find(trainId);
    if (it != trains.end()) {
        return it->second;
    }
    throw std::out_of_range("Train not found");
}

void InMemoryTrainRepository::clear() {
    trains.clear();
    next_id= 1;
    std::cout << "All trains destroyed\n";
}
