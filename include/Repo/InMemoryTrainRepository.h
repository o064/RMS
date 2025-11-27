#ifndef RMS_INMEMORYTRAINREPOSITORY_H
#define RMS_INMEMORYTRAINREPOSITORY_H

#include "../models/Train.h"
#include "ITrainRepository.h"
#include <map>
#include <optional>


class InMemoryTrainRepository : public ITrainRepository {
private:
    std::map<int, Train> trains;
    int next_id = 1;

public:
    InMemoryTrainRepository() = default;
    ~InMemoryTrainRepository() override = default;

    std::vector<Train> getAllTrains() const override;
    bool deleteTrain(int trainId) override;
    void  save( Train& newTrain) override;
    std::optional<Train> getTrainById(const int& trainId) const  override;
    void clear() override;
};

#endif