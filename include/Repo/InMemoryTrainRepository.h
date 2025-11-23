#ifndef RMS_INMEMORYTRAINREPOSITORY_H
#define RMS_INMEMORYTRAINREPOSITORY_H

#include "../models/Train.h"
#include "ITrainRepository.h"
#include <map>
#include <memory>

class InMemoryTrainRepository : public ITrainRepository {
private:
    std::map<int, Train> trains;
    int next_id = 1;

public:
    InMemoryTrainRepository() = default;
    ~InMemoryTrainRepository() override = default;

    std::list<Train> getAllTrains() const override;
    bool deleteTrain(int trainId) override;
    Train  save( Train& newTrain) override;
    Train getTrainById(const int& trainId) const  override;
    void clear() override;
};

#endif