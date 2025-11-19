//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_INMEMORYTRAINREPOSATORY_H
#define RMS_INMEMORYTRAINREPOSATORY_H

#include "../models/Train.h"
#include "ITrainReposatory.h"
#include <map>
class InMemoryTrainReposatory : public ITrainRepository{
private:
    std::map<int,Train> trains ;
public:
      std::vector<Train> getAllTrains() override;
      bool deleteTrain(int) override;
      Train save(Train) override;
      Train* getTrainById(int) override;
};
#endif //RMS_INMEMORYTRAINREPOSATORY_H
