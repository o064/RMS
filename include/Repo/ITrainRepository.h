#ifndef RMS_ITRAINREPOSITORY_H
#define RMS_ITRAINREPOSITORY_H

#include "../models/Train.h"
#include <list>
#include <memory>

class ITrainRepository {
public:
    virtual std::list<Train> getAllTrains() const = 0;
    virtual bool deleteTrain(int) = 0;
    virtual void save(Train&) = 0;
    virtual Train getTrainById(const int& trainId) const   = 0;
    virtual void clear() = 0;
    virtual ~ITrainRepository() = default;
};

#endif