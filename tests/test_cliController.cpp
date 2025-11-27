//
// CLIController_test.cpp
// Google Test suite for CLIController (Integration Tests)
//

#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include "CLIController.h"
#include "RMSFacade.h"
#include "Services/TrainService.h"
#include "Services/PassengerService.h"
#include "Services/TicketService.h"
#include "Repo/InMemoryTrainRepository.h"
#include "Repo/InMemoryPassengerRepository.h"
#include "Repo/InMemoryTicketRepository.h"

// Test fixture for CLIController tests
class CLIControllerTest : public ::testing::Test {
protected:
    // Repositories
    std::unique_ptr<InMemoryTrainRepository> trainRepo;
    std::unique_ptr<InMemoryPassengerRepository> passengerRepo;
    std::unique_ptr<InMemoryTicketRepository> ticketRepo;

    // Services
    std::unique_ptr<TrainService> trainService;
    std::unique_ptr<PassengerService> passengerService;
    std::unique_ptr<TicketService> ticketService;

    // Facade and Controller
    std::unique_ptr<RMSFacade> facade;
    std::unique_ptr<CLIController> controller;

    // I/O streams
    std::stringstream outputStream;
    std::streambuf* originalCout;

    void SetUp() override {
        // Create repositories
        trainRepo = std::make_unique<InMemoryTrainRepository>();
        passengerRepo = std::make_unique<InMemoryPassengerRepository>();
        ticketRepo = std::make_unique<InMemoryTicketRepository>();

        // Create services
        trainService = std::make_unique<TrainService>(trainRepo.get());
        passengerService = std::make_unique<PassengerService>(passengerRepo.get());
        ticketService = std::make_unique<TicketService>(
                ticketRepo.get(),
                trainService.get(),
                passengerService.get()
        );

        // Create facade
        facade = std::make_unique<RMSFacade>(
                trainService.get(),
                ticketService.get(),
                passengerService.get()
        );

        // Create controller
        controller = std::make_unique<CLIController>(facade.get());

        // Redirect cout
        originalCout = std::cout.rdbuf(outputStream.rdbuf());
    }

    void TearDown() override {
        // Restore cout
        std::cout.rdbuf(originalCout);

        // Clear repositories
        trainRepo->clear();
        passengerRepo->clear();
        ticketRepo->clear();
    }

    string getOutput() {
        return outputStream.str();
    }

    void clearOutput() {
        outputStream.str("");
        outputStream.clear();
    }
};

// ==================== Command Type Tests ====================

TEST_F(CLIControllerTest, GetCommandType_List) {
    EXPECT_EQ(CommandType::LIST, controller->getCommandType("list"));
}

TEST_F(CLIControllerTest, GetCommandType_Add) {
    EXPECT_EQ(CommandType::ADD, controller->getCommandType("add"));
}

TEST_F(CLIControllerTest, GetCommandType_Exit) {
    EXPECT_EQ(CommandType::EXIT, controller->getCommandType("exit"));
    EXPECT_EQ(CommandType::EXIT, controller->getCommandType("q"));
}

TEST_F(CLIControllerTest, GetCommandType_Help) {
    EXPECT_EQ(CommandType::HELP, controller->getCommandType("help"));
    EXPECT_EQ(CommandType::HELP, controller->getCommandType("h"));
    EXPECT_EQ(CommandType::HELP, controller->getCommandType("?"));
}

TEST_F(CLIControllerTest, GetCommandType_Train) {
    EXPECT_EQ(CommandType::TRAIN, controller->getCommandType("train"));
}

TEST_F(CLIControllerTest, GetCommandType_Book) {
    EXPECT_EQ(CommandType::BOOK, controller->getCommandType("book"));
}

TEST_F(CLIControllerTest, GetCommandType_Cancel) {
    EXPECT_EQ(CommandType::CANCEL, controller->getCommandType("cancel"));
}

TEST_F(CLIControllerTest, GetCommandType_Unknown) {
    EXPECT_EQ(CommandType::UNKNOWN, controller->getCommandType("invalid"));
    EXPECT_EQ(CommandType::UNKNOWN, controller->getCommandType("xyz"));
}

// ==================== Tokenize Tests ====================

TEST_F(CLIControllerTest, Tokenize_SingleWord) {
    vector<string> tokens = controller->tokenize("help");
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("help", tokens[0]);
}

TEST_F(CLIControllerTest, Tokenize_MultipleWords) {
    vector<string> tokens = controller->tokenize("list trains");
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("list", tokens[0]);
    EXPECT_EQ("trains", tokens[1]);
}

TEST_F(CLIControllerTest, Tokenize_WithExtraSpaces) {
    vector<string> tokens = controller->tokenize("  add   train   Express  ");
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("add", tokens[0]);
    EXPECT_EQ("train", tokens[1]);
    EXPECT_EQ("Express", tokens[2]);
}

TEST_F(CLIControllerTest, Tokenize_EmptyString) {
    vector<string> tokens = controller->tokenize("");
    EXPECT_EQ(0, tokens.size());
}

TEST_F(CLIControllerTest, Tokenize_ComplexCommand) {
    vector<string> tokens = controller->tokenize("book ticket 123 John");
    ASSERT_EQ(4, tokens.size());
    EXPECT_EQ("book", tokens[0]);
    EXPECT_EQ("ticket", tokens[1]);
    EXPECT_EQ("123", tokens[2]);
    EXPECT_EQ("John", tokens[3]);
}

// ==================== List Trains Tests ====================

TEST_F(CLIControllerTest, ListTrains_EmptyList) {
    controller->list_trains();

    string output = getOutput();
    EXPECT_TRUE(output.find("No trains avialble") != string::npos);
}

TEST_F(CLIControllerTest, ListTrains_WithTrains) {
    // Add a train first
    facade->addTrain("Express", 100);
    clearOutput();

    controller->list_trains();

    string output = getOutput();
    EXPECT_TRUE(output.find("Trains") != string::npos);
    EXPECT_TRUE(output.find("Express") != string::npos);
}

// ==================== List Passengers Tests ====================

TEST_F(CLIControllerTest, ListPassengers_EmptyList) {
    controller->list_passengers();

    string output = getOutput();
    EXPECT_TRUE(output.find("No passengers avialble") != string::npos);
}

TEST_F(CLIControllerTest, ListPassengers_WithPassengers) {
    // Add a passenger first
    facade->addPassenger("John");
    clearOutput();

    controller->list_passengers();

    string output = getOutput();
    EXPECT_TRUE(output.find("Passengers") != string::npos);
    EXPECT_TRUE(output.find("John") != string::npos);
}

// ==================== List Tickets Tests ====================

TEST_F(CLIControllerTest, ListTickets_EmptyList) {
    controller->list_tickets();

    string output = getOutput();
    EXPECT_TRUE(output.find("No tikcets avialble") != string::npos);
}

TEST_F(CLIControllerTest, ListTickets_WithTickets) {
    // Setup: create train and book ticket
    Train train = facade->addTrain("Express", 100);
    clearOutput();

    Ticket ticket = facade->bookTicket(train.getTrainId(), "Alice");
    clearOutput();

    controller->list_tickets();

    string output = getOutput();
    EXPECT_TRUE(output.find("Tickets") != string::npos);
    EXPECT_TRUE(output.find("Alice") != string::npos);
}

// ==================== Add Passenger Tests ====================

TEST_F(CLIControllerTest, AddPassenger_Success) {
    vector<string> args = {"add", "passenger", "John"};

    controller->add_passenger(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Passenger Added Successfully") != string::npos);
    EXPECT_TRUE(output.find("John") != string::npos);

    // Verify passenger was actually added
    auto passengers = facade->listPassengers();
    EXPECT_EQ(1, passengers.size());
    EXPECT_EQ("John", passengers[0].getName());
}

TEST_F(CLIControllerTest, AddPassenger_InvalidArgs) {
    vector<string> args = {"add", "passenger"};

    controller->add_passenger(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Usage") != string::npos);
}

TEST_F(CLIControllerTest, AddPassenger_MultipleName) {
    vector<string> args = {"add", "passenger", "John"};

    controller->add_passenger(args);
    clearOutput();

    // Try to add another passenger
    vector<string> args2 = {"add", "passenger", "Jane"};
    controller->add_passenger(args2);

    auto passengers = facade->listPassengers();
    EXPECT_EQ(2, passengers.size());
}

// ==================== Add Train Tests ====================

TEST_F(CLIControllerTest, AddTrain_Success) {
    vector<string> args = {"add", "train", "Express", "100"};

    controller->add_train(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Train Added Successfully") != string::npos);
    EXPECT_TRUE(output.find("Express") != string::npos);
    EXPECT_TRUE(output.find("100") != string::npos);

    // Verify train was actually added
    auto trains = facade->listTrains();
    EXPECT_EQ(1, trains.size());
    EXPECT_EQ("Express", trains[0].getTrainName());
    EXPECT_EQ(100, trains[0].getTotalSeats());
}

TEST_F(CLIControllerTest, AddTrain_InvalidArgs) {
    vector<string> args = {"add", "train", "Express"};

    controller->add_train(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Usage") != string::npos);
}

TEST_F(CLIControllerTest, AddTrain_InvalidSeats) {
    vector<string> args = {"add", "train", "Express", "invalid"};

    controller->add_train(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("ERROR") != string::npos);
}

// ==================== Book Ticket Tests ====================

TEST_F(CLIControllerTest, BookTicket_Success) {
    // Setup: create train
    Train train = facade->addTrain("Express", 100);
    clearOutput();

    vector<string> args = {"book", "ticket",
                           std::to_string(train.getTrainId()), "John"};

    controller->book_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Ticket Booked Successfully") != string::npos);
    EXPECT_TRUE(output.find("John") != string::npos);

    // Verify ticket and passenger were created
    auto tickets = facade->listTickets();
    EXPECT_EQ(1, tickets.size());

    auto passengers = facade->listPassengers();
    EXPECT_EQ(1, passengers.size());
    EXPECT_EQ("John", passengers[0].getName());
}

TEST_F(CLIControllerTest, BookTicket_InvalidArgs) {
    vector<string> args = {"book", "ticket", "123"};

    controller->book_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Usage") != string::npos);
}

TEST_F(CLIControllerTest, BookTicket_InvalidTrainId) {
    vector<string> args = {"book", "ticket", "invalid", "John"};

    controller->book_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("ERROR") != string::npos);
}

TEST_F(CLIControllerTest, BookTicket_TrainNotFound) {
    vector<string> args = {"book", "ticket", "999", "John"};

    controller->book_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("ERROR") != string::npos);
}

// ==================== Cancel Ticket Tests ====================

TEST_F(CLIControllerTest, CancelTicket_Success) {
    // Setup: create train and book ticket
    Train train = facade->addTrain("Express", 100);
    Ticket ticket = facade->bookTicket(train.getTrainId(), "John");
    clearOutput();

    vector<string> args = {"cancel", "ticket",
                           std::to_string(ticket.getId())};

    controller->cancel_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Ticket Cancelled Successfully") != string::npos);

    // Verify ticket was cancelled
    auto tickets = facade->listTickets();
    EXPECT_EQ(1, tickets.size());
    EXPECT_EQ(cancelled, tickets[0].getStatus());
}

TEST_F(CLIControllerTest, CancelTicket_InvalidArgs) {
    vector<string> args = {"cancel", "ticket"};

    controller->cancel_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Usage") != string::npos);
}

TEST_F(CLIControllerTest, CancelTicket_InvalidTicketId) {
    vector<string> args = {"cancel", "ticket", "invalid"};

    controller->cancel_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("ERROR") != string::npos);
}

TEST_F(CLIControllerTest, CancelTicket_TicketNotFound) {
    vector<string> args = {"cancel", "ticket", "999"};

    controller->cancel_ticket(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("ERROR") != string::npos);
}

// ==================== Train Availability Tests ====================

TEST_F(CLIControllerTest, GetTrainAvailability_Available) {
    // Create train with seats
    Train train = facade->addTrain("Express", 100);
    clearOutput();

    vector<string> args = {"train", "availability",
                           std::to_string(train.getTrainId())};

    controller->get_train_availability(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("This train is available") != string::npos);
}

TEST_F(CLIControllerTest, GetTrainAvailability_Full) {
    // Create train with 1 seat and book it
    Train train = facade->addTrain("Express", 1);
    facade->bookTicket(train.getTrainId(), "John");
    clearOutput();

    vector<string> args = {"train", "availability",
                           std::to_string(train.getTrainId())};

    controller->get_train_availability(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("This train is full") != string::npos);
}

TEST_F(CLIControllerTest, GetTrainAvailability_InvalidArgs) {
    vector<string> args = {"train", "availability"};

    controller->get_train_availability(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Usage") != string::npos);
}

TEST_F(CLIControllerTest, GetTrainAvailability_InvalidId) {
    vector<string> args = {"train", "availability", "invalid"};

    controller->get_train_availability(args);

    string output = getOutput();
    EXPECT_TRUE(output.find("Error") != string::npos);
}

// ==================== Show Help Test ====================

TEST_F(CLIControllerTest, ShowHelp) {
    controller->show_help();

    string output = getOutput();
    EXPECT_TRUE(output.find("RMS - Available Commands") != string::npos);
    EXPECT_TRUE(output.find("list trains") != string::npos);
    EXPECT_TRUE(output.find("add passenger") != string::npos);
    EXPECT_TRUE(output.find("book ticket") != string::npos);
    EXPECT_TRUE(output.find("cancel ticket") != string::npos);
}

// ==================== Integration Tests ====================

TEST_F(CLIControllerTest, Integration_CompleteWorkflow) {
    // 1. Add train
    vector<string> addTrainArgs = {"add", "train", "Express", "50"};
    controller->add_train(addTrainArgs);
    clearOutput();

    // 2. List trains
    controller->list_trains();
    string output = getOutput();
    EXPECT_TRUE(output.find("Express") != string::npos);
    clearOutput();

    // 3. Book tickets
    auto trains = facade->listTrains();
    ASSERT_EQ(1, trains.size());
    int trainId = trains[0].getTrainId();

    vector<string> bookArgs = {"book", "ticket",
                               std::to_string(trainId), "Alice"};
    controller->book_ticket(bookArgs);
    clearOutput();

    // 4. List tickets
    controller->list_tickets();
    output = getOutput();
    EXPECT_TRUE(output.find("Alice") != string::npos);
    clearOutput();

    // 5. List passengers
    controller->list_passengers();
    output = getOutput();
    EXPECT_TRUE(output.find("Alice") != string::npos);
}

