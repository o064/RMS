#ifndef RMS_ITRAINREPOSITORY_H
#define RMS_ITRAINREPOSITORY_H

#include "../models/Train.h"
#include <vector>
#include <memory>

class ITrainRepository {
public:
    virtual std::vector<std::shared_ptr<Train>> getAllTrains() const = 0;
    virtual bool deleteTrain(int) = 0;
    virtual std::shared_ptr<Train> save(std::shared_ptr<Train>) = 0;
    virtual std::shared_ptr<Train> getTrainById(int) = 0;
    virtual void clear() = 0;
    virtual ~ITrainRepository() = default;
};

#endif