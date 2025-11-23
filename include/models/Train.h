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
    Train(const int& id, const std::string& name, const int& totalSeats = 10);
    ~Train() = default;

    //  copy operations for unique ptr
    std::unique_ptr<Train> clone() const ;
    Train(const Train& other);
    Train& operator=(const Train& other);

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