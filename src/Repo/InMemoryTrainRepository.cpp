#include "Repo/InMemoryTrainRepository.h"
#include <iostream>

std::vector<std::shared_ptr<Train>> InMemoryTrainRepository::getAllTrains() const {
    std::vector<std::shared_ptr<Train>> result;
    for (const auto& train : trains) {
        result.push_back(train.second);
    }
    return result;
}

std::shared_ptr<Train> InMemoryTrainRepository::save(std::shared_ptr<Train> newTrain) {
    // assign id if needed
    if (newTrain->getTrainId() == 0) {
        newTrain->setTrainId(next_id++);
    }

    int id = newTrain->getTrainId();
    trains[id] = newTrain;

    if(id >= next_id) {
        next_id = id + 1;
    }
    return newTrain;
}

bool InMemoryTrainRepository::deleteTrain(int trainId) {
    auto it = trains.find(trainId);
    if (it != trains.end()) {
        trains.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<Train> InMemoryTrainRepository::getTrainById(int trainId) {
    auto it = trains.find(trainId);
    if (it != trains.end()) {
        return it->second;
    }
    return nullptr;
}

void InMemoryTrainRepository::clear() {
    trains.clear();
    std::cout << "All trains destroyed\n";
}