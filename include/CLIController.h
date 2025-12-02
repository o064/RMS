//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_CLICONTROLLER_H
#define RMS_CLICONTROLLER_H

#include "RMSFacade.h"
#include <string>
#include <iostream>
using namespace std;

class CLIController {
private:
    RMSFacade* facade;

public:
    CLIController(RMSFacade* facade);
    //helpers to manipulate text
    static vector<string> tokenize(const string &args) ;
    static string readLine();
    static  void waitUser();
    // run
     void run();
     static void clear();
     // help
     static void show_help();

    // train commands
    void list_trains();
    void add_train(const vector<string>& args);
    void get_train_availability(const vector<string>& args);
    void update_train(const vector<string>& args);
    void add_seats(const vector<string>& args);
    void delete_train(const vector<string>& args);
    void get_train_status(const vector<string>& args);

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
