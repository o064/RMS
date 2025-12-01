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
    if(cmd == "update") return CommandType::UPDATE;
    if(cmd == "delete") return CommandType::DELETE;
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
    cout << "\n===============================================\n";
    cout << "         RMS - Railway Management System        \n";
    cout << "===============================================\n\n";

    cout << "Usage: <command> [arguments]\n\n";

    cout << "-------------- Management Commands --------------\n";
    cout << "  list trains                                   \n";
    cout << "  list passengers                               \n";
    cout << "  list tickets                                  \n";
    cout << "  add train <name> <seats>                      \n";
    cout << "  add passenger <name>                          \n";
    cout << "  add seats <trainId|trainName> <seats>         \n";
    cout << "  update train <train_id> <seats> <name>        \n\n";
    cout << "  update passenger <passenger_id> <newName>     \n\n";
    cout << "  delete train <train_id>                        \n\n";
    cout << "  delete passenger <passenger_id>               \n\n";

    cout << "---------------- Booking Commands ----------------\n";
    cout << "  book ticket <train_id> <passenger_name>       - Book a ticket\n";
    cout << "  cancel ticket <ticket_id>                     - Cancel a ticket\n";
    cout << "  train availability <train_id>                 - Check seats\n\n";

    cout << "------------------ System Commands ---------------\n";
    cout << "  help / h / ?                                  \n";
    cout << "  exit / quit / q                               \n";
    cout << "--------------------------------------------------\n\n";
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
    cout << "Under the supervision of Dr.Iman and Eng/Mariem Abdelrahman?\n\n";
}
void CLIController::run() {
    sayWelcome();
    while(true){
        cout << "\ncmd >  ";
        string command =readLine();
        vector<string> args = tokenize(command);
        if(args.empty()){
            cout << "Please enter a command. Type 'help' for available commands.\n";
            continue;

        }
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
                else if (args[1]=="seats")
                    add_seats(args);
                else
                    cout << "Unknown add target.\n";
                break;

                // ===================== BOOK ============================
            case CommandType::BOOK:
                book_ticket(args);
                break;
                // ===================== UPDATE ============================
            case CommandType::UPDATE:
                if (args.size() < 2) {
                    cout << "Usage: update [train|passenger]\n";
                    break;
                }
                if (args[1] == "train")
                    update_train(args);
                else if(args[1] =="passenger")
                    update_passenger(args);
                else
                    cout << "Unknown update target.\n";
                break;

                // ===================== Delete ============================
            case CommandType::DELETE:
                if (args.size() < 2) {
                    cout << "Usage: delete [train|passenger]\n";
                    break;
                }
                if (args[1] == "train")
                    delete_train(args);
                else if(args[1] =="passenger")
                    delete_passenger(args);
                else
                    cout << "Unknown delete target.\n";
                break;
                // ===================== CANCEL ==========================
            case CommandType::CANCEL:
                cancel_ticket(args);
                break;
            // ===================== Exit ==========================

            case CommandType::EXIT:
                cout<< "GoodBye..........! \n";
                exit(0);
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
     string name;
    try{
        name = combineString(args,2);
        auto p = facade->addPassenger(name);
        printPassengerInfo(p ,"--- Passenger Added Successfully ---\n");

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

void CLIController::add_train(const vector<string> &args) {
    if(args.size() < 4) {
        cout << "Usage: add train <name> <seats> \n";
        return;
    }
    const string& seatArgs = args[3];
    string name;
    try {
        const int &seats = parseInt(seatArgs,"seats");
        name = combineString(args,2,-1);

        auto t = facade->addTrain(name,seats);
        printTrainInfo(t,"--- Train Added Successfully ---\n");

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
     string passengerName ;

    try {
        int trainId = parseInt(trainIdArg, "train ID");
        passengerName = combineString(args,3);

        auto t = facade->bookTicket(trainId, passengerName);
        printTicketInfo(t,"--- Ticket Booked Successfully ---\n");

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

void CLIController::update_train(const vector<string> &args) {

    if(args.size() < 5) {
        cout << "Usage: update train <train_id> <name> <seats>\n";
        return;
    }

    const string& tarinIdArg = args[2];
    const string& seatArg = args[4];
    try {
        int trainId = parseInt(tarinIdArg , "train id");
        int seats = parseInt(seatArg, "seat number");
        string name = combineString(args,3,-1);
        auto t =facade->updateTrain(trainId , name , seats);

        printTrainInfo(t , "------ Train updated successfully ------ \n");


    } catch (const exception& e) {
        cout << "ERROR: Could not updateTrain " << tarinIdArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::add_seats(const vector<string> &args) {
    if(args.size() < 5) {
        cout << "Usage: add seats <trainId|trainName> <seats>\n";
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
            printTrainInfo(t , "--- Train updated successfully. --- \n");

        }else{  // add seats trainName
            string name = combineString(args,2,-1); // until last field
            const auto &t =facade->addSeats(name, seats);
            printTrainInfo(t , "Train updated successfully.");

        }



    } catch (const exception& e) {
        cout << "ERROR: Could not update Train " << trainArg << ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::update_passenger(const vector<string> &args) {
    if(args.size() < 5) {
        cout << "Usage: update passenger <passengerId> <newName>\n";
        return;
    }

    const string& passengerArg = args[2];
    try {
        int passengerId = parseInt(passengerArg , "passenger Id");
        string name = combineString(args,3);
        auto p = facade->updatePassenger(passengerId, name);
        printPassengerInfo(p , "------ Passenger updated successfully ------ \n");

    } catch (const exception& e) {
        cout << "ERROR: Could not update passenger " << passengerArg<< ".\n";
        cout << "Details: " << e.what() << "\n";
    }
}

void CLIController::delete_train(const vector<string> &args) {
    if(args.size() < 3) {
        cout << "Usage: delete train <trainId>\n";
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
        cout << "Usage: delete passenger <passengerId>\n";
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


