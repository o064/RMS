//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_CLICONTROLLER_H
#define RMS_CLICONTROLLER_H

#include "RMSFacade.h"
#include <string>
#include <iostream>
using namespace std;
enum CommandType{
    LIST,
    ADD,
    BOOK,
    CANCEL,
    TRAIN,
    HELP,
    EXIT,
    UNKNOWN
};
class CLIController {
private:
    RMSFacade* facade;
public:
    CLIController(RMSFacade* facade);

     void run();
    CommandType getCommandType(const string& cmd);
    vector<string> tokenize(const string &args);
    string readLine();

    void show_help();

    void list_trains();
    void add_train(const vector<string>& args);

    void add_passenger(const vector<string>& args);
    void list_passengers();

    void list_tickets();

    void cancel_ticket(const vector<string>& args);
    void book_ticket(const vector<string>& args);
    void get_train_availability(const vector<string>& args);


};
#endif //RMS_CLICONTROLLER_H
