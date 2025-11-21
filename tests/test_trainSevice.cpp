//
// Created by Omar on 11/21/2025.
//
// TrainServiceTest.cpp
#include "gtest/gtest.h"
#include "Services/TrainService.h"
#include "Repo/InMemoryTrainRepository.h"
#include "models/Train.h"

// Fixture for TrainService tests
class TrainServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<InMemoryTrainRepository> repo;
    std::unique_ptr<TrainService> service;

    void SetUp() override {
        repo = std::make_unique<InMemoryTrainRepository>();
        service = std::make_unique<TrainService>(repo.get());
    }

    void TearDown() override {
        repo->clear();  // optional cleanup
    }
};

// Test creating a train
TEST_F(TrainServiceTest, CreateTrain) {
    auto train = service->createTrain("Express", 5);

    ASSERT_NE(train, nullptr);
    EXPECT_EQ(train->getTrainName(), "Express");
    EXPECT_EQ(train->getTotalSeats(), 5);
    EXPECT_GT(train->getTrainId(), 0);
}

// Test getting a train by ID
TEST_F(TrainServiceTest, GetTrainById) {
    auto train = service->createTrain("Local", 10);
    int id = train->getTrainId();

    auto fetched = service->getTrain(id);
    ASSERT_NE(fetched, nullptr);
    EXPECT_EQ(fetched->getTrainId(), id);
    EXPECT_EQ(fetched->getTrainName(), "Local");
}

// Test deleting a train
TEST_F(TrainServiceTest, DeleteTrain) {
    auto train = service->createTrain("Rapid", 8);
    int id = train->getTrainId();

    bool deleted = service->deleteTrain(id);
    EXPECT_TRUE(deleted);

    auto fetched = service->getTrain(id);
    EXPECT_EQ(fetched, nullptr); // should be gone
}

// Test getAllTrains
TEST_F(TrainServiceTest, GetAllTrains) {
    service->createTrain("Train1", 5);
    service->createTrain("Train2", 6);

    auto allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 2);
}

// Test seat availability
TEST_F(TrainServiceTest, SeatAvailability) {
    auto train = service->createTrain("SeatsTest", 3);
    int id = train->getTrainId();

    EXPECT_TRUE(service->isAvailbleSeat(id));

    auto allocator = train->getSeatAllocator();
    allocator->allocateSeat(1);
    allocator->allocateSeat(2);
    allocator->allocateSeat(3);

    // all seats allocated
    EXPECT_FALSE(service->isAvailbleSeat(id));

    allocator->freeSeat(2);  // free one seat
    EXPECT_TRUE(service->isAvailbleSeat(id));
}
