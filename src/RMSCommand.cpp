//
// Created by Omar on 12/1/2025.
//
#include "RMSCommand.h"

RMSCommand::MainCmd RMSCommand::getMainCmd(const string &cmd){
    static const unordered_map<string, MainCmd> table = {
            {"train", MainCmd::TRAIN},
            {"passenger", MainCmd::PASSENGER},
            {"ticket", MainCmd::TICKET},
            {"help", MainCmd::SYSTEM},
            {"h", MainCmd::SYSTEM},
            {"?", MainCmd::SYSTEM},
            {"exit", MainCmd::SYSTEM},
            {"quit", MainCmd::SYSTEM},
            {"q", MainCmd::SYSTEM},

    };
    auto it = table.find(cmd);
    return (it != table.end()) ? it->second : MainCmd::UNKNOWN;
}

RMSCommand::TrainCmd RMSCommand::getTrainCmd(const string &sub) {
    static const unordered_map<string, TrainCmd> table = { // static map to initializing only once
            {"list", TrainCmd::LIST},
            {"add", TrainCmd::ADD},
            {"delete", TrainCmd::DELETE},
            {"update", TrainCmd::UPDATE},
            {"seats", TrainCmd::SEATS_ADD},
            {"availability", TrainCmd::AVAILABILITY}
    };
    auto it = table.find(sub);
    return (it != table.end()) ? it->second : TrainCmd::UNKNOWN;
}
RMSCommand::PassengerCmd RMSCommand::getPassengerCmd(const string &sub)  {
    static const unordered_map<string, PassengerCmd> table = {
            {"list", PassengerCmd::LIST},
            {"add", PassengerCmd::ADD},
            {"delete", PassengerCmd::DELETE},
            {"update", PassengerCmd::UPDATE}
    };
    auto it = table.find(sub);
    return (it != table.end()) ? it->second : PassengerCmd::UNKNOWN;
}
RMSCommand::TicketCmd RMSCommand::getTicketCmd(const string &sub) {
    static const unordered_map<string, TicketCmd> table = {
            {"list", TicketCmd::LIST},
            {"book", TicketCmd::BOOK},
            {"cancel", TicketCmd::CANCEL}
    };
    auto it = table.find(sub);
    return (it != table.end()) ? it->second : TicketCmd::UNKNOWN;
}


RMSCommand::SystemCmd RMSCommand::getSystemCmd(const string &cmd) {
    if(cmd == "help" || cmd == "h" || cmd == "?") return SystemCmd::HELP;
    if(cmd == "exit" || cmd == "quit" || cmd == "q") return SystemCmd::EXIT;
    return SystemCmd::UNKNOWN;
}
