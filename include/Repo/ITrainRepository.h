#ifndef RMS_ITRAINREPOSITORY_H
#define RMS_ITRAINREPOSITORY_H

#include "../models/Train.h"
#include <vector>
#include <optional>

class ITrainRepository {
public:
    virtual std::vector<Train> getAllTrains() const = 0;
    virtual bool deleteTrain(int) = 0;
    virtual void save(Train&) = 0;
    virtual std::optional<Train> getTrainById(const int& trainId) const   = 0;
    virtual void clear() = 0;
    virtual ~ITrainRepository() = default;
};

#endif