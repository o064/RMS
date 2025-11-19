//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_CLICONTROLLER_H
#define RMS_CLICONTROLLER_H

#include "RMSFacade.h"
#include <string>
class CLIController {
private:
    RMSFacade* facade;


public:
    CLIController(RMSFacade* facade);
    void run();

    void show_help(std::string command);
    void list_train(std::string command);
    void add_train(std::string command);
    void add_passenger(std::string command);
    void list_tickets(std::string command);
    void cancel_ticket(std::string command);
    void book_ticket(std::string command);
    void get_train_availability(std::string command);
};
#endif //RMS_CLICONTROLLER_H
