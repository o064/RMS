//
// Created by Omar on 11/25/2025.
//

#include <sstream>
#include <iomanip> // required for setw()
#include "CLIController.h"
#include "utils/helpers.h" // parseInt , trim

CommandType CLIController::getCommandType(const string& cmd){
    if(cmd == "list") return CommandType::LIST;
    if(cmd == "add") return CommandType::ADD;
    if(cmd == "exit" || cmd == "q") return CommandType::EXIT;
    if(cmd == "help"|| cmd == "h" || cmd == "?") return CommandType::HELP;
    if(cmd == "train") return CommandType::TRAIN;
    if(cmd == "book") return CommandType::BOOK;
    if(cmd == "cancel") return CommandType::CANCEL;
    return CommandType::UNKNOWN;

}
vector<string> CLIController::tokenize(const string &args) { // split the sentence to args
    stringstream ss(args);
    vector<string> tokens ;
    string token;
    while(ss >> token){
        tokens.push_back(token);
    }
    return tokens;

}

string CLIController::readLine() {
    string line;
    getline(cin ,line);
    return line;
}

void CLIController::show_help() {
    cout << "RMS - Available Commands:\n";
    cout << "Management:\n";
    cout << " list trains\n";
    cout << " list passengers\n";
    cout << " list tickets\n";
    cout << " add train <name> <seats>\n";
    cout << " add passenger <name>          - Show all trains - Show all passengers - Show all tickets - Add a new train - Add a new passenger\n";
    cout << "Booking:\n";
    cout << " book ticket <train_id> <passenger_name> - Book a ticket\n";
    cout << " cancel ticket <ticket_id>     - Cancel a ticket\n";
    cout << " train availability <train_id> - Check available seats\n";
    cout << "System:\n";
    cout << " help / h / ?\n";
    cout << " exit / quit / q\n";
}

void CLIController::list_trains() {
    const auto trains = facade->listTrains();
    if(trains.empty())
        cout << "No trains avialble \n";
    cout << "\n======== Trains ==========\n";
    cout << left << setw(6) << "ID"
         << left << setw(15) << "Name"
         << left << setw(20) << "Status"
         << "\n";

    for (auto &train : trains) {

        int seats = train.getSeatAllocator()->getAvailableSeatCount();

        string status = (seats == 0)
                        ? "Full"
                        : to_string(seats) + " seats available";

        cout << left << setw(6)  << train.getTrainId()
             << left << setw(15) << train.getTrainName()
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
        cout << "No tikcets avialble \n";
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

void CLIController::run() {
    cout << "===============================================\n";
    cout << "  Welcome to the Railway Reservation System  \n";
    cout << "===============================================\n";
    cout << "type help to show commands \n";
    cout << "-----------------------------------------------\n";
    cout << "How can I assist you today?\n\n";
    while(true){
        cout << "\nDr.Eman >  ";
        string command =readLine();
        vector<string> args = tokenize(command);
        if(args.empty())
            continue;
        CommandType cmdType = getCommandType(args[0]);
        switch (cmdType) {

            // ===================== LIST ============================
            case CommandType::LIST:
                if (args.size() < 2) {
                    cout << "Usage: list [trains|passengers|tickets]\n";
                    break;
                }

                if (args[1] == "trains")         list_trains();
                else if (args[1] == "passengers") list_passengers();
                else if (args[1] == "tickets")    list_tickets();
                else
                    cout << "Unknown list target.\n";
                break;

                // ===================== HELP ============================
            case CommandType::HELP:
                show_help();
                break;

                // ===================== TRAIN ===========================
            case CommandType::TRAIN:
                get_train_availability(args);
                break;

                // ===================== ADD =============================
            case CommandType::ADD:
                if (args.size() < 2) {
                    cout << "Usage: add [train|ticket]\n";
                    break;
                }

                if (args[1] == "train")
                    add_train(args);
                else if (args[1] == "passenger")
                    add_passenger(args);
                else
                    cout << "Unknown add target.\n";
                break;

                // ===================== BOOK ============================
            case CommandType::BOOK:
                book_ticket(args);
                break;

                // ===================== CANCEL ==========================
            case CommandType::CANCEL:
                cancel_ticket(args);
                break;
            // ===================== CANCEL ==========================

            case CommandType::EXIT:
                cout<< "GoodBye..........! \n";
                return;
                // ===================== UNKNOWN =========================
            default:
                cout << "Unknown command. Type 'help' for available commands.\n";

        }

    }
}

void CLIController::add_passenger(const vector<string> &args) {
//add passenger <name>
    if(args.size() < 3) {
        cout << "Usage: add passenger <name>\n";
        return;
    }
    const string& name = args[2];
    try {
        auto p = facade->addPassenger(name);
        cout << "--- Passenger Added Successfully ---\n";
        cout << "ID: " << p.getId() << "\n";
        cout << "Name: " << p.getName() << "\n";
        cout << "------------------------------------\n";
    } catch(const exception& e) {
        cout << "ERROR: Could not add passenger " << name << ". " << e.what() << "\n";
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
    }catch(const exception& e){
        cout << "Error while get_train_availability : " << e.what() << endl;
        return;
    }

    bool isFull = facade->getTrainAvailability(id);
    if(isFull)
        cout << "This train is full \n";
    else
        cout << "This train is available \n";

}

void CLIController::add_train(const vector<string> &args) {
    if(args.size() < 4) {
        cout << "Usage: add train <name> <seats>\n";
        return;
    }
    const string& name = args[2];
    try {
        const int &seats = parseInt(args[3],"seats");
        auto t = facade->addTrain(name,seats);
        cout << "--- Train Added Successfully ---\n";
        cout << "Name: " << t.getTrainName() << "\n";
        cout << "Total Seats: " << t.getTotalSeats() << "\n";
        cout << "--------------------------------\n";
    }
    catch (const exception& e) {
        cout << "ERROR: Could not add train " << name << ". " << e.what() << "\n";
    }
}
void CLIController::book_ticket(const vector<string> &args) {
    if(args.size() < 4) { // book ticket <train_id> <passenger_name>
        cout << "Usage: book ticket <train_id> <passenger_name>\n";
        return;
    }

    const string& trainIdArg = args[2];
    const string& passengerName = args[3];

    try {
        int trainId = parseInt(trainIdArg, "train ID");

        auto t = facade->bookTicket(trainId, passengerName);

        cout << "--- Ticket Booked Successfully ---\n";
        cout << "Ticket ID: " << t.getId() << "\n";
        cout << "Train ID: " << t.getTrainId() << "\n";
        cout << "Passenger: " << t.getPassenger().getName() << "\n";
        cout << "----------------------------------\n";

    } catch (const exception& e) {
        cout << "ERROR: Could not book ticket for " << passengerName << " on Train ID " << trainIdArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::cancel_ticket(const vector<string> &args) {
    if(args.size() < 3) { // cancel ticket <ticket_id>
        cout << "Usage: cancel ticket <ticket_id>\n";
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