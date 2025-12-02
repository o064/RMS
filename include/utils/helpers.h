//
// Created by Omar on 11/24/2025.
//

#ifndef RMS_HELPERS_H
#define RMS_HELPERS_H
#include <string>
#include <vector>
#include "models/Train.h"
#include "models/Passenger.h"
#include "models/Ticket.h"

// string validation
std::string toLowerCase(std::string word);
std::string trim(const std::string &str);
bool isValidName(const std::string& name);
// string concat
std::string combineString(const std::vector<std::string>& args, int start);
std::string combineString(const std::vector<std::string>& args, int start , int end);
bool compareString(const std::string& str1 , const std::string& str2);
// integer helpers
bool isInteger(const std::string& str);
int parseInt(const std::string& arg , const std::string& argName);

// print msg

void printCurrentDate();

#endif //RMS_HELPERS_H
