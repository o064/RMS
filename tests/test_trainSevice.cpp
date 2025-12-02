#include <gtest/gtest.h>
#include "Services/TrainService.h"
#include "Repo/InMemoryTrainRepository.h"

class TrainServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<InMemoryTrainRepository> repo;
    std::unique_ptr<TrainService> service;

    void SetUp() override {
        repo = std::make_unique<InMemoryTrainRepository>();
        service = std::make_unique<TrainService>(repo.get());
    }
};

TEST_F(TrainServiceTest, CreateTrain_Success) {
    Train train = service->createTrain("Test Train", 30);
    EXPECT_GT(train.getTrainId(), 0);
    EXPECT_EQ(train.getTrainName(), "Test Train");
    EXPECT_EQ(train.getTotalSeats(), 30);
}

TEST_F(TrainServiceTest, GetTrain_Exists) {
    Train created = service->createTrain("Test Train", 30);
    auto retrieved = service->getTrain(created.getTrainId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getTrainName(), "Test Train");
}

TEST_F(TrainServiceTest, GetTrain_NotExists) {
    EXPECT_THROW(service->getTrain(999), std::runtime_error);
}

TEST_F(TrainServiceTest, GetAllTrains_Empty) {
    auto trains = service->getAllTrains();
    EXPECT_TRUE(trains.empty());
}

TEST_F(TrainServiceTest, GetAllTrains_Multiple) {
    service->createTrain("Train 1", 20);
    service->createTrain("Train 2", 30);
    service->createTrain("Train 3", 40);

    auto trains = service->getAllTrains();
    EXPECT_EQ(trains.size(), 3);
}

TEST_F(TrainServiceTest, UpdateTrain_Success) {
    Train created = service->createTrain("Old Name", 20);
    Train updated = service->updateTrain(created.getTrainId(), "New Name", 30);

    EXPECT_EQ(updated.getTrainName(), "New Name");
    EXPECT_EQ(updated.getTotalSeats(), 30);
}

TEST_F(TrainServiceTest, UpdateTrain_NotExists) {
    EXPECT_THROW(service->updateTrain(999, "Test", 30), std::runtime_error);
}

TEST_F(TrainServiceTest, DeleteTrain_Success) {
    Train created = service->createTrain("Test", 20);
    bool deleted = service->deleteTrain(created.getTrainId());
    EXPECT_TRUE(deleted);
}

TEST_F(TrainServiceTest, DeleteTrain_NotExists) {
    bool deleted = service->deleteTrain(999);
    EXPECT_FALSE(deleted);
}

TEST_F(TrainServiceTest, AddSeats_ById_Success) {
    Train created = service->createTrain("Test", 20);
    Train updated = service->addSeats(created.getTrainId(), 10);
    EXPECT_EQ(updated.getTotalSeats(), 30);
}

TEST_F(TrainServiceTest, AddSeats_ByName_Success) {
    service->createTrain("Test Train", 20);
    Train updated = service->addSeats("Test Train", 10);
    EXPECT_EQ(updated.getTotalSeats(), 30);
}

TEST_F(TrainServiceTest, AddSeats_ByName_NotExists) {
    EXPECT_THROW(service->addSeats("NonExistent", 10), std::runtime_error);
}

TEST_F(TrainServiceTest, IsAvailableSeat_True) {
    Train created = service->createTrain("Test", 20);
    EXPECT_TRUE(service->isAvailbleSeat(created.getTrainId()));
}

TEST_F(TrainServiceTest, IsAvailableSeat_NotExists) {
    EXPECT_THROW(service->isAvailbleSeat(999), std::runtime_error);
}