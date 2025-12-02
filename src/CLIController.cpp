//
// Created by Omar on 11/25/2025.
//

#include <sstream>
#include <iomanip> // required for setw()
#include <limits> // for numeric_limits
#include <cstdlib> // Required for system()

#include "CLIController.h"
#include "utils/helpers.h" // parseInt , trim
#include "RMSCommand.h"

using std::cout ;
using std::endl;
using std::string;
using std::cin;

//enums [MainCmd instead of RMSCommand::MainCmd
using MainCmd = RMSCommand::MainCmd;
using TrainCmd = RMSCommand::TrainCmd;
using PassengerCmd = RMSCommand::PassengerCmd;
using TicketCmd = RMSCommand::TicketCmd;
using SystemCmd = RMSCommand::SystemCmd;
// getters
// pointer fo functions
auto getMainCmd =  RMSCommand::getMainCmd;
auto getTrainCmd =  RMSCommand::getTrainCmd;
auto getPassengerCmd=  RMSCommand::getPassengerCmd;
auto getTicketCmd = RMSCommand::getTicketCmd;
auto getSystemCmd =RMSCommand::getSystemCmd;


vector<string> CLIController::tokenize(const string &args) { // split the sentence to args
    stringstream ss(args);
    vector<string> tokens ;
    string token;
    while(ss >> token){
        tokens.push_back(token);
    }
    return tokens;
}
void CLIController::waitUser(){
//    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //  clear buffer to ingnore '\n'
    cout << "Press Enter to exit...";
    cin.get(); // wait for Enter
}
string CLIController::readLine() {
    string line;
    getline(cin ,line);
    return line;
}

void CLIController::show_help() {
    cout << "\n=================================================\n";
    cout << "        RMS - Railway Management System          \n";
    cout << "=================================================\n\n";

    cout << "Usage: <command> [arguments]\n\n";

    cout << "---------------------- Commands ----------------------\n\n";


    // ======================== TRAIN ========================
    cout << "train:\n";
    cout << "   train list                                     - Show all trains\n";
    cout << "   train add <name> <seats>                       - Add a new train\n";
    cout << "   train delete <id>                              - Remove a train\n";
    cout << "   train update <id> <seats> <name>               - Update a train\n";
    cout << "   train seats add <id> <count>                   - Increase seat count\n";
    cout << "   train availability <id>                        - Show seat status\n\n";
    cout << "   train status <id>                              - Show full train status\n\n";

    // ====================== PASSENGER ======================
    cout << "passenger:\n";
    cout << "   passenger list                                 - Show all passengers\n";
    cout << "   passenger add <name>                           - Add a passenger\n";
    cout << "   passenger delete <id>                          - Remove a passenger\n";
    cout << "   passenger update <id> <newName>                - Rename passenger\n\n";

    // ======================== TICKET ========================
    cout << "ticket:\n";
    cout << "   ticket list                                    - Show all tickets\n";
    cout << "   ticket book <trainId> <passengerId|name>       - Book a ticket\n";
    cout << "   ticket cancel <ticketId>                       - Cancel a ticket\n\n";

    // ========================= SYSTEM ========================
    cout << "system:\n";
    cout << "   help | h | ?                                   - Show help menu\n";
    cout << "   exit | quit | q                                - Exit program\n";
    cout << "   clear                                          - Clear Screen\n";

    cout << "--------------------------------------------------------\n\n";
}



void CLIController::list_trains() {
    const auto trains = facade->listTrains();
    if(trains.empty())
        cout << "No trains avialble \n";
    cout << "\n======== Trains ==========\n";
    cout << left << setw(6) << "ID"
         << left << setw(20) << "Name"
         << left << setw(20) << "Status"
         << "\n";

    for (auto &train : trains) {

        int seats = train.getSeatAllocator()->getAvailableSeatCount();

        string status = (seats == 0)
                        ? "Full"
                        : to_string(seats) + " seats available";

        cout << left << setw(6)  << train.getTrainId()
             << left << setw(20) << train.getTrainName()
             << left << setw(20) << status
             << "\n";
    }

}

void CLIController::list_passengers() {
    const auto passengers = facade->listPassengers();
    if(passengers.empty())
        cout << "No passengers avialble \n";
    cout << "\n======== Passengers ==========\n\n";
    cout << left << setw(6) << "ID"
         << left << setw(20) << "Name" << "\n";

    for (auto &passenger : passengers) {
        cout << left << setw(6) << passenger.getId()
             << left << setw(20) << passenger.getName()
             << "\n";
    }
}

void CLIController::list_tickets() {
    const auto tickets = facade->listTickets();
    if(tickets.empty())
        cout << "No tickets available \n";
    cout << "\n====================================== Tickets =======================================\n";

    cout << left
         << setw(8)  << "id"
         << setw(12) << "trainId"
         << setw(15) << "ticket_seat"
         << setw(15) << "passengerId"
         << setw(20) << "passenger_name"
         << setw(15) << "status"
         << endl;

    for (auto &ticket : tickets) {
        cout << left
             << setw(8)  << ticket.getId()
             << setw(12) << ticket.getTrainId()
             << setw(15) << ticket.getSeat()
             << setw(15) << ticket.getPassenger().getId()
             << setw(20) << ticket.getPassenger().getName()
             << setw(15) << ((ticket.getStatus() == booked) ? "Booked" : "Cancelled")
             << endl;
    }
}





CLIController::CLIController(RMSFacade *facade) :facade(facade){

}
void sayWelcome(){
    cout << "===============================================\n";
    cout << "  Welcome to the Railway Reservation System  \n";
    cout << "===============================================\n";
    cout << "Type 'help' to show the list of commands.\n";
    cout << "-----------------------------------------------\n";
    printCurrentDate();
    cout << "-----------------------------------------------\n";
    cout << "Author : Dangerous Team .\n";
    cout << "Under the supervision of Dr.Iman and Eng/Mariem Abdelrahman.\n\n";
}
void CLIController::run() {
    clear();
    sayWelcome();
//lambad function
    auto printUsage = [](MainCmd cmd) {
        switch(cmd) {
            case MainCmd::TRAIN:      cout << "Usage: train <list|add|delete|update|seats|availability>\n"; break;
            case MainCmd::PASSENGER:  cout << "Usage: passenger <list|add|delete|update>\n"; break;
            case MainCmd::TICKET:     cout << "Usage: ticket <list|book|cancel>\n"; break;
            default:                  cout << "Unknown command. Type 'help' for available commands.\n";
        }
    };

    while (true) {
        cout << "\ncmd > ";
        string line = readLine();
        vector<string> args = tokenize(line);

        if (args.empty()) {
            cout << "Please enter a command. Type 'help' for available commands.\n";
            continue;
        }

        MainCmd mainCmd = getMainCmd(args[0]);

        switch (mainCmd) {

            // ===================== TRAIN =====================
            case MainCmd::TRAIN: {
                if (args.size() < 2) { printUsage(mainCmd); break; }
                TrainCmd tCmd = getTrainCmd(args[1]);
                switch (tCmd) {
                    case TrainCmd::LIST:         list_trains(); break;
                    case TrainCmd::ADD:          add_train(args); break;
                    case TrainCmd::DELETE:       delete_train(args); break;
                    case TrainCmd::UPDATE:       update_train(args); break;
                    case TrainCmd::SEATS_ADD:    add_seats(args); break;
                    case TrainCmd::AVAILABILITY: get_train_availability(args); break;
                    case TrainCmd::STATUS: get_train_status(args); break;

                    default: cout << "Invalid train command.\n";
                }
                break;
            }

                // ===================== PASSENGER =====================
            case MainCmd::PASSENGER: {
                if (args.size() < 2) { printUsage(mainCmd); break; }
                PassengerCmd pCmd = getPassengerCmd(args[1]);
                switch (pCmd) {
                    case PassengerCmd::LIST:   list_passengers(); break;
                    case PassengerCmd::ADD:    add_passenger(args); break;
                    case PassengerCmd::DELETE: delete_passenger(args); break;
                    case PassengerCmd::UPDATE: update_passenger(args); break;
                    default: cout << "Invalid passenger command.\n";
                }
                break;
            }

                // ===================== TICKET =====================
            case MainCmd::TICKET: {
                if (args.size() < 2) { printUsage(mainCmd); break; }
                TicketCmd tkCmd = getTicketCmd(args[1]);
                switch (tkCmd) {
                    case TicketCmd::LIST:   list_tickets(); break;
                    case TicketCmd::BOOK:   book_ticket(args); break;
                    case TicketCmd::CANCEL: cancel_ticket(args); break;
                    default: cout << "Invalid ticket command.\n";
                }
                break;
            }

                // ===================== SYSTEM =====================
            case MainCmd::SYSTEM: {
                SystemCmd sCmd = getSystemCmd(args[0]);
                switch (sCmd) {
                    case SystemCmd::HELP: show_help(); break;
                    case SystemCmd::CLEAR :
                        clear();
                        sayWelcome();
                        break;
                    case SystemCmd::EXIT:
                        cout << "Goodbye!\n";
                        waitUser();
                        return;
                    default: cout << "Invalid system command.\n";
                }
                break;
            }

                // ===================== UNKNOWN =====================
            default:
                cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
}

void CLIController::add_passenger(const vector<string> &args) {
//add passenger <name>
    if(args.size() < 3) {
        cout << "Usage: passenger add  <name>\n";

        return;
    }
     string name;
    try{
        name = combineString(args,2);
        auto p = facade->addPassenger(name);
        p.print("--- Passenger Added Successfully ---\n");

    } catch(const exception& e) {
        cout << "ERROR: Could not add passenger " << name << ". " << "\n";
        cout << "Details: " << e.what() << "\n";

    }
}

void CLIController::get_train_availability(const vector<string> &args) {
//    train availability <train_id>
    if(args.size() < 3) {
        cout << "Usage: train availability <train_id>\n";
        return;
    }
    int id ;
    try{
        id = parseInt(args[2],"id");
        bool isAvailable = facade->getTrainAvailability(id);
        if(!isAvailable)
            cout << "This train is full \n";
        else
            cout << "This train is available \n";

    }catch(const exception& e){
        cout << "Error while get_train_availability : " << e.what() << endl;
        return;
    }



}
void CLIController::clear(){
    system("cls");
}

void CLIController::add_train(const vector<string> &args) {
    if(args.size() < 4) {
        cout << "Usage: train add  <name> <seats> \n";
        return;
    }
    const string& seatArgs = args[3];
    string name;
    try {
        const int &seats = parseInt(seatArgs,"seats");
        name = combineString(args,2,-1);

        auto t = facade->addTrain(name,seats);
        t.print("--- Train Added Successfully ---\n");

    }
    catch (const exception& e) {
        cout << "ERROR: Could not add train " << name << ". " << e.what() << "\n";
    }
}
void CLIController::book_ticket(const vector<string> &args) {
    if(args.size() < 4) { // book ticket <train_id> <passenger_name>
        cout << "Usage: ticket book  <train_id> <passenger_name>\n";
        return;
    }

    const string& trainIdArg = args[2];
     string passengerName ;

    try {
        int trainId = parseInt(trainIdArg, "train ID");
        passengerName = combineString(args,3);

        auto t = facade->bookTicket(trainId, passengerName);
        if(t.has_value())
            t->print("--- Ticket Booked Successfully ---\n");
        else
            cout << "train is full so the passenger added to the waiting list \n";

    } catch (const exception& e) {
        cout << "ERROR: Could not book ticket for " << passengerName << " on Train ID " << trainIdArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::cancel_ticket(const vector<string> &args) {
    if(args.size() < 3) { // cancel ticket <ticket_id>
        cout << "Usage: ticket cancel  <ticket_id>\n";
        return;
    }

    const string& ticketIdArg = args[2];

    try {
        int ticketId = parseInt(ticketIdArg, "ticket ID");
        facade->cancelTicket(ticketId);

        cout << "--- Ticket Cancelled Successfully ---\n";
        cout << "Ticket ID: " << ticketId << " has been cancelled.\n";
        cout << "-------------------------------------\n";

    } catch (const exception& e) {
        // Error Message
        cout << "ERROR: Could not cancel ticket " << ticketIdArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::update_train(const vector<string> &args) {

    if(args.size() < 5) {
        cout << "Usage: train update  <train_id> <name> <seats>\n";
        return;
    }

    const string& tarinIdArg = args[2];
    const string& seatArg = args[4];
    try {
        int trainId = parseInt(tarinIdArg , "train id");
        int seats = parseInt(seatArg, "seat number");
        string name = combineString(args,3,-1);
        auto t =facade->updateTrain(trainId , name , seats);

        t.print( "------ Train updated successfully ------ \n");


    } catch (const exception& e) {
        cout << "ERROR: Could not updateTrain " << tarinIdArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::add_seats(const vector<string> &args) {
    if(args.size() < 5) {
        cout << "Usage: train seats  <trainId|trainName> <seats>\n";
        return;
    }

    const string& trainArg = args[2];
    const string& seatArg = args[3];
    try {
        int seats = parseInt(seatArg, "seat number");
        // add seats trainId
        if(isInteger(trainArg)){
            int trainId = parseInt(trainArg , "train id");
            const auto &t = facade->addSeats(trainId, seats);
            t.print( "--- Train updated successfully. --- \n");

        }else{  // add seats trainName
            string name = combineString(args,2,-1); // until last field
            const auto &t =facade->addSeats(name, seats);
            t.print( "Train updated successfully.\n");

        }



    } catch (const exception& e) {
        cout << "ERROR: Could not update Train " << trainArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::update_passenger(const vector<string> &args) {
    if(args.size() < 5) {
        cout << "Usage: passenger update <passengerId> <newName>\n";
        return;
    }

    const string& passengerArg = args[2];
    try {
        int passengerId = parseInt(passengerArg , "passenger Id");
        string name = combineString(args,3);
        auto p = facade->updatePassenger(passengerId, name);
        p.print( "------ Passenger updated successfully ------ \n");

    } catch (const exception& e) {
        cout << "ERROR: Could not update passenger " << passengerArg<< ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::delete_train(const vector<string> &args) {
    if(args.size() < 3) {
        cout << "Usage: train delete  <trainId>\n";
        return;
    }
    const string& trainArg = args[2];
    try {
        int trainId = parseInt(trainArg,"train Id");
        facade->deleteTrain(trainId);
        cout << "train with id " << trainId << " deleted successfully \n";
    }catch (const exception& e) {
        cout << "ERROR: Could not delete train" << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::delete_passenger(const vector<string> &args) {
    if(args.size() < 3) {
        cout << "Usage: passenger delete  <passengerId>\n";
        return;
    }
    const string& passengerArg = args[2];
    try {
        int passengerId = parseInt(passengerArg,"train Id");
        facade->deletePassenger(passengerId);
        cout << "passenger with id " << passengerId << " deleted successfully \n";
    }catch (const exception& e) {
        cout << "ERROR: Could not delete passenger" << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::get_train_status(const vector<string> &args) {
    if(args.size() < 3) {
        cout << "Usage: train status  <trainId>\n";
        return;
    }
    const string& trainArg = args[2];
    try {
        int trainId = parseInt(trainArg,"train Id");
        facade->trainStatus(trainId);
    }catch (const exception& e) {
        cout << "ERROR: could not print train data with id : " << trainArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}


