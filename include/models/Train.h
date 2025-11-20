#ifndef RMS_TRAIN_H
#define RMS_TRAIN_H

#include "SeatAllocator.h"
#include <string>
#include <memory>

class Train {
private:
    int id;
    std::string name;
    int totalSeats;
    std::unique_ptr<SeatAllocator> seatAllocator;

public:
    // Constructor
    Train(int id, const std::string& name, int totalSeats = 10);
    ~Train() = default;

    // Delete copy operations
    Train(const Train&) = delete;
    Train& operator=(const Train&) = delete;

    // move operations
    Train(Train&&) = default;
    Train& operator=(Train&&) = default;

    int getTrainId() const;
    std::string getTrainName() const;
    SeatAllocator* getSeatAllocator() const;

    void setTrainName(const std::string& name);
    void setTrainId(int trainId);

    bool hasAvailableSeats() const;

    int getTotalSeats() const;
};

#endif //RMS_TRAIN_H