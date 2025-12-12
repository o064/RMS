#include <gtest/gtest.h>
#include "CLIController.h"
#include "RMSFacade.h"
#include "StartupManager.h"
#include <sstream>

class CLIControllerTest : public ::testing::Test
{
protected:
    std::unique_ptr<StartupManager> startupManager;
    RMSFacade *facade;
    std::unique_ptr<CLIController> controller;

    void SetUp() override
    {
        startupManager = std::make_unique<StartupManager>();
        facade = startupManager->buildFacade();
        controller = std::make_unique<CLIController>(facade);
    }

    void TearDown() override
    {
        // Cleanup handled by unique_ptr
    }
};

// ===================== Tokenize Tests =====================

TEST_F(CLIControllerTest, Tokenize_SingleWord)
{
    auto tokens = CLIController::tokenize("help");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0], "help");
}

TEST_F(CLIControllerTest, Tokenize_MultipleWords)
{
    auto tokens = CLIController::tokenize("train add Express 20");
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "train");
    EXPECT_EQ(tokens[1], "add");
    EXPECT_EQ(tokens[2], "Express");
    EXPECT_EQ(tokens[3], "20");
}

TEST_F(CLIControllerTest, Tokenize_ExtraSpaces)
{
    auto tokens = CLIController::tokenize("  train   list  ");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "train");
    EXPECT_EQ(tokens[1], "list");
}

TEST_F(CLIControllerTest, Tokenize_EmptyString)
{
    auto tokens = CLIController::tokenize("");
    EXPECT_TRUE(tokens.empty());
}

TEST_F(CLIControllerTest, Tokenize_OnlySpaces)
{
    auto tokens = CLIController::tokenize("     ");
    EXPECT_TRUE(tokens.empty());
}

TEST_F(CLIControllerTest, Tokenize_WithNumbers)
{
    auto tokens = CLIController::tokenize("ticket book 1 John");
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "ticket");
    EXPECT_EQ(tokens[1], "book");
    EXPECT_EQ(tokens[2], "1");
    EXPECT_EQ(tokens[3], "John");
}

TEST_F(CLIControllerTest, Tokenize_CommandWithName)
{
    auto tokens = CLIController::tokenize("passenger add John Doe");
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "passenger");
    EXPECT_EQ(tokens[1], "add");
    EXPECT_EQ(tokens[2], "John");
    EXPECT_EQ(tokens[3], "Doe");
}

// ===================== Train Command Tests =====================

TEST_F(CLIControllerTest, ListTrains_InitialState)
{
    // Should list the 5 mock trains loaded by StartupManager
    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->list_trains();

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Trains"), std::string::npos);
}

TEST_F(CLIControllerTest, AddTrain_ValidInput)
{
    vector<std::string> args = {"train", "add", "TestTrain", "25"};

    EXPECT_NO_THROW(controller->add_train(args));

    auto trains = facade->listTrains();
    EXPECT_EQ(trains.size(), 6); // 5 mock + 1 new
}

TEST_F(CLIControllerTest, AddTrain_InsufficientArgs)
{
    vector<std::string> args = {"train", "add", "TestTrain"};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->add_train(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Usage"), std::string::npos);
}

TEST_F(CLIControllerTest, AddTrain_InvalidSeats)
{
    vector<std::string> args = {"train", "add", "TestTrain", "invalid"};

    std::ostringstream output;
    std::streambuf *oldCerr = std::cerr.rdbuf(output.rdbuf());

    controller->add_train(args);

    std::cerr.rdbuf(oldCerr);

    std::string result = output.str();
    EXPECT_NE(result.find("ERROR"), std::string::npos);
}

TEST_F(CLIControllerTest, AddTrain_MultiWordName)
{
    vector<std::string> args = {"train", "add", "Super", "Express", "Train", "25"};

    EXPECT_NO_THROW(controller->add_train(args));

    auto trains = facade->listTrains();
    bool found = false;
    for (const auto &train : trains)
    {
        if (train.getTrainName() == "Super Express Train")
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(CLIControllerTest, DeleteTrain_ValidId)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "train", "delete", std::to_string(trains[0].getTrainId())};

    EXPECT_NO_THROW(controller->delete_train(args));

    auto updatedTrains = facade->listTrains();
    EXPECT_EQ(updatedTrains.size(), trains.size() - 1);
}

TEST_F(CLIControllerTest, DeleteTrain_InsufficientArgs)
{
    vector<std::string> args = {"train", "delete"};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->delete_train(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Usage"), std::string::npos);
}

TEST_F(CLIControllerTest, UpdateTrain_ValidInput)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "train", "update", std::to_string(trains[0].getTrainId()),
        "UpdatedName", "30"};

    EXPECT_NO_THROW(controller->update_train(args));
}

TEST_F(CLIControllerTest, AddSeats_ValidInput)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "train", "seats", std::to_string(trains[0].getTrainId()), "10"};

    EXPECT_NO_THROW(controller->add_seats(args));
}

TEST_F(CLIControllerTest, GetTrainAvailability_Available)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "train", "availability", std::to_string(trains[0].getTrainId())};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->get_train_availability(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("available"), std::string::npos);
}

TEST_F(CLIControllerTest, GetTrainStatus_ValidId)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "train", "status", std::to_string(trains[0].getTrainId())};

    EXPECT_NO_THROW(controller->get_train_status(args));
}

// ===================== Passenger Command Tests =====================

TEST_F(CLIControllerTest, ListPassengers_InitialState)
{
    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->list_passengers();

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Passengers"), std::string::npos);
}

TEST_F(CLIControllerTest, AddPassenger_ValidInput)
{
    vector<std::string> args = {"passenger", "add", "TestPassenger"};

    EXPECT_NO_THROW(controller->add_passenger(args));

    auto passengers = facade->listPassengers();
    EXPECT_EQ(passengers.size(), 11); // 10 mock + 1 new
}

TEST_F(CLIControllerTest, AddPassenger_MultiWordName)
{
    vector<std::string> args = {"passenger", "add", "John", "Doe"};

    EXPECT_NO_THROW(controller->add_passenger(args));

    auto passengers = facade->listPassengers();
    bool found = false;
    for (const auto &p : passengers)
    {
        if (p.getName() == "John Doe")
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(CLIControllerTest, AddPassenger_InsufficientArgs)
{
    vector<std::string> args = {"passenger", "add"};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->add_passenger(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Usage"), std::string::npos);
}

TEST_F(CLIControllerTest, UpdatePassenger_ValidInput)
{
    auto passengers = facade->listPassengers();
    ASSERT_FALSE(passengers.empty());

    vector<std::string> args = {
        "passenger", "update", std::to_string(passengers[0].getId()),
        "UpdatedName"};

    EXPECT_NO_THROW(controller->update_passenger(args));
}

TEST_F(CLIControllerTest, DeletePassenger_ValidId)
{
    Passenger temp = facade->addPassenger("TempPassenger");

    vector<std::string> args = {
        "passenger", "delete", std::to_string(temp.getId())};

    EXPECT_NO_THROW(controller->delete_passenger(args));
}

// ===================== Ticket Command Tests =====================

TEST_F(CLIControllerTest, ListTickets_InitialState)
{
    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->list_tickets();

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Tickets"), std::string::npos);
}

TEST_F(CLIControllerTest, BookTicket_ValidInput)
{
    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    vector<std::string> args = {
        "ticket", "book", std::to_string(trains[0].getTrainId()),
        "NewPassenger"};

    EXPECT_NO_THROW(controller->book_ticket(args));
}

TEST_F(CLIControllerTest, BookTicket_InsufficientArgs)
{
    vector<std::string> args = {"ticket", "book", "1"};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->book_ticket(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Usage"), std::string::npos);
}

TEST_F(CLIControllerTest, BookTicket_InvalidTrainId)
{
    vector<std::string> args = {"ticket", "book", "invalid", "John"};

    std::ostringstream output;
    std::streambuf *oldCerr = std::cerr.rdbuf(output.rdbuf());

    controller->book_ticket(args);

    std::cerr.rdbuf(oldCerr);

    std::string result = output.str();
    EXPECT_NE(result.find("ERROR"), std::string::npos);
}

TEST_F(CLIControllerTest, CancelTicket_ValidId)
{
    auto tickets = facade->listTickets();
    ASSERT_FALSE(tickets.empty());

    vector<std::string> args = {
        "ticket", "cancel", std::to_string(tickets[0].getId())};

    EXPECT_NO_THROW(controller->cancel_ticket(args));
}

TEST_F(CLIControllerTest, CancelTicket_InsufficientArgs)
{
    vector<std::string> args = {"ticket", "cancel"};

    std::ostringstream output;
    std::streambuf *oldCout = std::cout.rdbuf(output.rdbuf());

    controller->cancel_ticket(args);

    std::cout.rdbuf(oldCout);

    std::string result = output.str();
    EXPECT_NE(result.find("Usage"), std::string::npos);
}

// ===================== Helper Function Tests =====================

TEST_F(CLIControllerTest, ReadLine_ReturnsString)
{
    // Simulate stdin
    std::istringstream input("test input\n");
    std::streambuf *oldCin = std::cin.rdbuf(input.rdbuf());

    std::string result = CLIController::readLine();

    std::cin.rdbuf(oldCin);

    EXPECT_EQ(result, "test input");
}

// ===================== Edge Cases =====================

TEST_F(CLIControllerTest, HandleEmptyCommand)
{
    vector<std::string> args = {};

    // Commands with empty args should handle gracefully
    EXPECT_NO_THROW(controller->add_train(args));
    EXPECT_NO_THROW(controller->add_passenger(args));
    EXPECT_NO_THROW(controller->book_ticket(args));
}

TEST_F(CLIControllerTest, HandleInvalidNumbers)
{
    vector<std::string> args = {"train", "add", "Test", "abc"};

    std::ostringstream output;
    std::streambuf *oldCerr = std::cerr.rdbuf(output.rdbuf());

    controller->add_train(args);

    std::cerr.rdbuf(oldCerr);

    std::string result = output.str();
    EXPECT_NE(result.find("ERROR"), std::string::npos);
}

TEST_F(CLIControllerTest, HandleNegativeNumbers)
{
    vector<std::string> args = {"train", "add", "Test", "-5"};

    std::ostringstream output;
    std::streambuf *oldCerr = std::cerr.rdbuf(output.rdbuf());

    controller->add_train(args);

    std::cerr.rdbuf(oldCerr);

    std::string result = output.str();
    EXPECT_NE(result.find("ERROR"), std::string::npos);
}

// ===================== Integration Tests =====================

TEST_F(CLIControllerTest, CompleteWorkflow)
{
    // Add train
    vector<std::string> trainArgs = {"train", "add", "WorkflowTrain", "10"};
    controller->add_train(trainArgs);

    // Add passenger
    vector<std::string> passengerArgs = {"passenger", "add", "WorkflowPassenger"};
    controller->add_passenger(passengerArgs);

    // Get the train ID
    auto trains = facade->listTrains();
    int trainId = -1;
    for (const auto &train : trains)
    {
        if (train.getTrainName() == "WorkflowTrain")
        {
            trainId = train.getTrainId();
            break;
        }
    }
    ASSERT_GT(trainId, 0);

    // Book ticket
    vector<std::string> ticketArgs = {
        "ticket", "book", std::to_string(trainId), "WorkflowPassenger"};
    controller->book_ticket(ticketArgs);

    // Verify
    auto tickets = facade->listTickets();
    bool found = false;
    for (const auto &ticket : tickets)
    {
        if (ticket.getTrainId() == trainId &&
            ticket.getPassenger().getName() == "WorkflowPassenger")
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}