#ifndef RMS_INMEMORYTRAINREPOSITORY_H
#define RMS_INMEMORYTRAINREPOSITORY_H

#include "../models/Train.h"
#include "ITrainRepository.h"
#include <map>
#include <memory>

class InMemoryTrainRepository : public ITrainRepository {
private:
    std::map<int, std::shared_ptr<Train>> trains;
    int next_id = 1;

public:
    InMemoryTrainRepository() = default;
    ~InMemoryTrainRepository() override = default;

    std::vector<std::shared_ptr<Train>> getAllTrains() const override;
    bool deleteTrain(int trainId) override;
    std::shared_ptr<Train> save(std::shared_ptr<Train> newTrain) override;
    std::shared_ptr<Train> getTrainById(int trainId) override;
    void clear() override;
};

#endif