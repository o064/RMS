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
    UPDATE,
    BOOK,
    CANCEL,
    TRAIN,
    DELETE,
    HELP,
    EXIT,
    UNKNOWN
};
class CLIController {
private:
    RMSFacade* facade;

public:
    CLIController(RMSFacade* facade);
    CommandType getCommandType(const string& cmd);
    void show_help();
    vector<string> tokenize(const string &args) ;
    string readLine();

     void run();


    // train commands
    void list_trains();
    void add_train(const vector<string>& args);
    void get_train_availability(const vector<string>& args);
    void update_train(const vector<string>& args);
    void add_seats(const vector<string>& args);
    void delete_train(const vector<string>& args);

    // passenger commands

    void add_passenger(const vector<string>& args);
    void update_passenger(const vector<string>& args);
    void delete_passenger(const vector<string>& args);
    void list_passengers();

    // ticket commands
    void list_tickets();

    void cancel_ticket(const vector<string>& args);
    void book_ticket(const vector<string>& args);



};
#endif //RMS_CLICONTROLLER_H
