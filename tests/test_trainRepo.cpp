#include "gtest/gtest.h"
#include "Repo/InMemoryTrainRepository.h"
#include <memory>
#include <algorithm>  // For std::find

// Helper to create a train
std::shared_ptr<Train> createTrain(int id, const std::string& name, int seats) {
    return std::make_shared<Train>(id, name, seats);
}

class InMemoryTrainRepositoryTest : public ::testing::Test {
protected:
    InMemoryTrainRepository repo;

    void SetUp() override {
        repo.clear();
    }

    void TearDown() override {
        repo.clear();
    }
};

// Test saving and retrieving by ID
TEST_F(InMemoryTrainRepositoryTest, SaveAndGetTrainById) {
    auto t1 = createTrain(0, "Express", 20);
    auto saved1 = repo.save(t1);

    ASSERT_NE(saved1, nullptr);
    EXPECT_EQ(saved1->getTrainName(), "Express");
    EXPECT_EQ(saved1->getSeatAllocator()->getAvailableSeatCount(), 20);

    auto fetched = repo.getTrainById(saved1->getTrainId());
    ASSERT_NE(fetched, nullptr);
    EXPECT_EQ(fetched->getTrainName(), "Express");
}

// Test auto-increment ID
TEST_F(InMemoryTrainRepositoryTest, AutoIncrementId) {
    auto t1 = createTrain(0, "Express", 20);
    auto saved1 = repo.save(t1);
    auto t2 = createTrain(0, "Regional", 15);
    auto saved2 = repo.save(t2);

    EXPECT_EQ(saved1->getTrainId(), 1);
    EXPECT_EQ(saved2->getTrainId(), 2);
}

// Test getAllTrains
TEST_F(InMemoryTrainRepositoryTest, GetAllTrains) {
    repo.save(createTrain(0, "Express", 20));
    repo.save(createTrain(0, "Regional", 15));

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);

    std::vector<std::string> names;
    for (auto& t : all) {
        names.push_back(t->getTrainName());
    }

    EXPECT_NE(std::find(names.begin(), names.end(), std::string("Express")), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), std::string("Regional")), names.end());
}

// Test deleteTrain
TEST_F(InMemoryTrainRepositoryTest, DeleteTrain) {
    auto t = repo.save(createTrain(0, "Express", 20));
    int id = t->getTrainId();

    bool deleted = repo.deleteTrain(id);
    EXPECT_TRUE(deleted);

    auto fetched = repo.getTrainById(id);
    EXPECT_EQ(fetched, nullptr);
}

// Test clear
TEST_F(InMemoryTrainRepositoryTest, ClearRepository) {
    repo.save(createTrain(0, "Express", 20));
    repo.save(createTrain(0, "Regional", 15));

    repo.clear();
    auto all = repo.getAllTrains();
    EXPECT_TRUE(all.empty());
}

// Test SeatAllocator functionality
TEST_F(InMemoryTrainRepositoryTest, SeatAllocatorWorks) {
    auto t = repo.save(createTrain(0, "Express", 5));
    auto seatAllocator = t->getSeatAllocator();

    ASSERT_NE(seatAllocator, nullptr);
    EXPECT_TRUE(t->hasAvailableSeats());
    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 5);

    // Allocate all seats
    for (int i = 1; i <= 5; ++i) {
        int seatNumber = seatAllocator->allocateSeat(i);  // i is passenger ID
        EXPECT_GT(seatNumber, 0);  // Should return a valid seat number
    }

    EXPECT_FALSE(t->hasAvailableSeats());
    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 0);
}

// Test seat allocation and freeing
TEST_F(InMemoryTrainRepositoryTest, SeatAllocationAndFreeing) {
    auto t = repo.save(createTrain(0, "Test Train", 3));
    auto seatAllocator = t->getSeatAllocator();

    // Allocate seats
    int seat1 = seatAllocator->allocateSeat(101);
    int seat2 = seatAllocator->allocateSeat(102);

    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 1);

    // Free a seat
    seatAllocator->freeSeat(seat1);
    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 2);

    // Allocate again
    int seat3 = seatAllocator->allocateSeat(103);
    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 1);
}

// Test updating existing train
TEST_F(InMemoryTrainRepositoryTest, UpdateExistingTrain) {
    auto t1 = createTrain(0, "Express", 20);
    auto saved1 = repo.save(t1);
    int originalId = saved1->getTrainId();

    // Create updated train with same ID
    auto updatedTrain = createTrain(originalId, "Super Express", 30);
    auto saved2 = repo.save(updatedTrain);

    EXPECT_EQ(saved2->getTrainId(), originalId);
    EXPECT_EQ(saved2->getTrainId(), originalId);
    EXPECT_EQ(saved2->getTrainName(), "Super Express");
    EXPECT_EQ(saved2->getSeatAllocator()->getAvailableSeatCount(), 30);

    // Verify only one train exists
    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 1);
}

// Test get non-existent train
TEST_F(InMemoryTrainRepositoryTest, GetNonExistentTrain) {
    auto result = repo.getTrainById(999);
    EXPECT_EQ(result, nullptr);
}

// Test delete non-existent train
TEST_F(InMemoryTrainRepositoryTest, DeleteNonExistentTrain) {
    bool result = repo.deleteTrain(999);
    EXPECT_FALSE(result);
}

// Test train ID persistence
TEST_F(InMemoryTrainRepositoryTest, TrainIdPersistence) {
    // Train with explicit ID
    auto t1 = createTrain(100, "Special Train", 25);
    auto saved1 = repo.save(t1);
    EXPECT_EQ(saved1->getTrainId(), 100);

    // Next auto-generated ID should be higher
    auto t2 = createTrain(0, "Auto Train", 15);
    auto saved2 = repo.save(t2);
    EXPECT_EQ(saved2->getTrainId(), 101);
}

// Test multiple operations
TEST_F(InMemoryTrainRepositoryTest, MultipleOperations) {
    // Add multiple trains
    auto t1 = repo.save(createTrain(0, "Train A", 10));
    auto t2 = repo.save(createTrain(0, "Train B", 15));
    auto t3 = repo.save(createTrain(0, "Train C", 20));

    // Verify all are present
    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 3);

    // Delete one
    bool deleted = repo.deleteTrain(t2->getTrainId());
    EXPECT_TRUE(deleted);

    // Verify remaining
    all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);

    // Verify the correct trains remain
    std::vector<std::string> names;
    for (auto& t : all) {
        names.push_back(t->getTrainName());
    }
    EXPECT_NE(std::find(names.begin(), names.end(), std::string("Train A")), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), std::string("Train C")), names.end());
    EXPECT_EQ(std::find(names.begin(), names.end(), std::string("Train B")), names.end());
}