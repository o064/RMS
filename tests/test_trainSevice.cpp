#include <gtest/gtest.h>
#include "Services/TrainService.h"
#include "Repo/InMemoryTrainRepository.h"
#include "models/Train.h"
#include <stdexcept>
#include <algorithm>
#include <optional>

class TrainServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<InMemoryTrainRepository> repo;
    std::unique_ptr<TrainService> service;

    void SetUp() override {
        repo = std::make_unique<InMemoryTrainRepository>();
        service = std::make_unique<TrainService>(repo.get());
    }

    void TearDown() override {
        repo->clear();
    }
};

// Test creating a train
TEST_F(TrainServiceTest, CreateTrain) {
    Train train = service->createTrain("Express", 5);

    EXPECT_EQ(train.getTrainName(), "Express");
    EXPECT_EQ(train.getTotalSeats(), 5);
    EXPECT_GT(train.getTrainId(), 0);
}

// Test creating multiple trains
TEST_F(TrainServiceTest, CreateMultipleTrains) {
    Train train1 = service->createTrain("Express", 5);
    Train train2 = service->createTrain("Local", 10);

    EXPECT_NE(train1.getTrainId(), train2.getTrainId());
    EXPECT_EQ(train1.getTrainName(), "Express");
    EXPECT_EQ(train2.getTrainName(), "Local");
}

// Test getting a train by ID
TEST_F(TrainServiceTest, GetTrainById) {
    Train train = service->createTrain("Local", 10);
    int id = train.getTrainId();

    auto fetchedOpt = service->getTrain(id);
    ASSERT_TRUE(fetchedOpt.has_value());
    EXPECT_EQ(fetchedOpt->getTrainId(), id);
    EXPECT_EQ(fetchedOpt->getTrainName(), "Local");
    EXPECT_EQ(fetchedOpt->getTotalSeats(), 10);
}

// Test getting non-existent train
TEST_F(TrainServiceTest, GetNonExistentTrain) {
    // Service propagates repo behavior, which throws runtime_error for not found
    EXPECT_THROW(service->getTrain(999), std::runtime_error);
}

// Test deleting a train
TEST_F(TrainServiceTest, DeleteTrain) {
    Train train = service->createTrain("Rapid", 8);
    int id = train.getTrainId();

    bool deleted = service->deleteTrain(id);
    EXPECT_TRUE(deleted);

    EXPECT_THROW(service->getTrain(id), std::runtime_error);
}

// Test deleting non-existent train
TEST_F(TrainServiceTest, DeleteNonExistentTrain) {
    bool deleted = service->deleteTrain(999);
    EXPECT_FALSE(deleted);
}

// Test getAllTrains
TEST_F(TrainServiceTest, GetAllTrains) {
    service->createTrain("Train1", 5);
    service->createTrain("Train2", 6);

    auto allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 2);
}

// Test getAllTrains returns empty list
TEST_F(TrainServiceTest, GetAllTrainsEmpty) {
    auto allTrains = service->getAllTrains();
    EXPECT_TRUE(allTrains.empty());
}

// Test seat availability - basic
TEST_F(TrainServiceTest, SeatAvailability) {
    Train train = service->createTrain("SeatsTest", 3);
    int id = train.getTrainId();

    EXPECT_TRUE(service->isAvailbleSeat(id));
}

// Test seat availability changes with allocation
TEST_F(TrainServiceTest, SeatAvailabilityWithAllocation) {
    Train train = service->createTrain("SeatsTest", 3);
    int id = train.getTrainId();

    // Allocate all seats directly through the repository
    auto trainRefOpt = repo->getTrainById(id);
    ASSERT_TRUE(trainRefOpt.has_value());
    Train trainRef = trainRefOpt.value();

    auto allocator = trainRef.getSeatAllocator();
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);
    allocator->allocateSeat(103);

    // Save the changes back to the repository
    repo->save(trainRef);

    // Now check - the service fetches a fresh copy from repo
    EXPECT_FALSE(service->isAvailbleSeat(id));
}

// Test seat availability for non-existent train
TEST_F(TrainServiceTest, SeatAvailabilityNonExistentTrain) {
    EXPECT_THROW(service->isAvailbleSeat(999), std::runtime_error);
}

// Test multiple service operations
TEST_F(TrainServiceTest, MultipleOperations) {
    Train t1 = service->createTrain("Train A", 10);
    Train t2 = service->createTrain("Train B", 15);

    auto allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 2);

    bool deleted = service->deleteTrain(t1.getTrainId());
    EXPECT_TRUE(deleted);

    allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 1);

    EXPECT_EQ(allTrains.front().getTrainName(), "Train B");
}

// Test create train with zero seats
TEST_F(TrainServiceTest, CreateTrainWithZeroSeats) {
    Train train = service->createTrain("Zero Seats", 0);

    EXPECT_EQ(train.getTotalSeats(), 0);
    EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 10);
}

// Test create train with negative seats
TEST_F(TrainServiceTest, CreateTrainWithNegativeSeats) {
    Train train = service->createTrain("Negative", -5);

    EXPECT_EQ(train.getTotalSeats(), -5);
    EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 10);
}

// Test create train with empty name
TEST_F(TrainServiceTest, CreateTrainEmptyName) {
    Train train = service->createTrain("", 10);

    EXPECT_EQ(train.getTrainName(), "");
    EXPECT_EQ(train.getTotalSeats(), 10);
}

// Test service doesn't own repository
TEST_F(TrainServiceTest, ServiceDoesNotOwnRepository) {
    InMemoryTrainRepository* rawRepo = repo.get();

    service->createTrain("Test", 5);

    service.reset();

    auto trains = rawRepo->getAllTrains();
    EXPECT_EQ(trains.size(), 1);
}

// Test getAllTrains returns copies
TEST_F(TrainServiceTest, GetAllTrainsReturnsCopies) {
    Train t1 = service->createTrain("Train1", 5);

    auto allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 1);

    Train& modifiedTrain = allTrains.front();
    modifiedTrain.setTrainName("Modified");

    auto originalOpt = service->getTrain(t1.getTrainId());
    ASSERT_TRUE(originalOpt.has_value());
    EXPECT_EQ(originalOpt->getTrainName(), "Train1");
}

// Test sequential train creation
TEST_F(TrainServiceTest, SequentialTrainCreation) {
    std::vector<int> ids;

    for (int i = 0; i < 5; ++i) {
        Train t = service->createTrain("Train" + std::to_string(i), 10 + i);
        ids.push_back(t.getTrainId());
    }

    std::sort(ids.begin(), ids.end());
    auto it = std::unique(ids.begin(), ids.end());
    EXPECT_EQ(it, ids.end());

    auto all = service->getAllTrains();
    EXPECT_EQ(all.size(), 5);
}

// Test delete and recreate
TEST_F(TrainServiceTest, DeleteAndRecreate) {
    Train t1 = service->createTrain("First", 10);
    int id1 = t1.getTrainId();

    service->deleteTrain(id1);

    Train t2 = service->createTrain("Second", 15);
    int id2 = t2.getTrainId();

    EXPECT_NE(id1, id2);

    auto all = service->getAllTrains();
    EXPECT_EQ(all.size(), 1);
    EXPECT_EQ(all.front().getTrainName(), "Second");
}

// Test isAvailbleSeat returns fresh data
TEST_F(TrainServiceTest, IsAvailbleSeatReturnsFreshData) {
    Train train = service->createTrain("Test", 2);
    int id = train.getTrainId();

    EXPECT_TRUE(service->isAvailbleSeat(id));

    auto fetchedOpt = service->getTrain(id);
    ASSERT_TRUE(fetchedOpt.has_value());
    Train fetched = fetchedOpt.value();
    auto allocator = fetched.getSeatAllocator();
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);

    repo->save(fetched);

    EXPECT_FALSE(service->isAvailbleSeat(id));
}

// Test creating many trains
TEST_F(TrainServiceTest, CreateManyTrains) {
    const int COUNT = 100;

    for (int i = 0; i < COUNT; ++i) {
        service->createTrain("Train" + std::to_string(i), i + 1);
    }

    auto all = service->getAllTrains();
    EXPECT_EQ(all.size(), COUNT);
}

// Test train with large seat count
TEST_F(TrainServiceTest, TrainWithLargeSeatCount) {
    Train train = service->createTrain("Large", 1000);

    EXPECT_EQ(train.getTotalSeats(), 1000);
    EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 1000);
}

// Test createTrain returns updated train with ID
TEST_F(TrainServiceTest, CreateTrainReturnsTrainWithId) {
    Train train = service->createTrain("Test", 10);

    EXPECT_GT(train.getTrainId(), 0);

    auto fetchedOpt = service->getTrain(train.getTrainId());
    ASSERT_TRUE(fetchedOpt.has_value());
    EXPECT_EQ(fetchedOpt->getTrainName(), "Test");
    EXPECT_EQ(fetchedOpt->getTotalSeats(), 10);
}

// Test that createTrain properly saves to repository
TEST_F(TrainServiceTest, CreateTrainSavesToRepository) {
    Train train = service->createTrain("Saved", 20);
    int id = train.getTrainId();

    auto fromRepoOpt = repo->getTrainById(id);
    ASSERT_TRUE(fromRepoOpt.has_value());
    EXPECT_EQ(fromRepoOpt->getTrainName(), "Saved");
    EXPECT_EQ(fromRepoOpt->getTotalSeats(), 20);
}

// Test concurrent train creation
TEST_F(TrainServiceTest, ConcurrentTrainCreation) {
    std::vector<Train> trains;

    for (int i = 0; i < 10; ++i) {
        trains.push_back(service->createTrain("Train" + std::to_string(i), i * 5));
    }

    std::set<int> uniqueIds;
    for (const auto& train : trains) {
        uniqueIds.insert(train.getTrainId());
    }
    EXPECT_EQ(uniqueIds.size(), 10);

    auto allTrains = service->getAllTrains();
    EXPECT_EQ(allTrains.size(), 10);
}

// Test train creation after repository clear
TEST_F(TrainServiceTest, CreateTrainAfterClear) {
    service->createTrain("First", 10);
    service->createTrain("Second", 15);

    repo->clear();

    Train train = service->createTrain("New", 20);
    EXPECT_EQ(train.getTrainId(), 1);

    auto all = service->getAllTrains();
    EXPECT_EQ(all.size(), 1);
}
