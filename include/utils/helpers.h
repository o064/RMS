//
// Created by Omar on 11/24/2025.
//

#ifndef RMS_HELPERS_H
#define RMS_HELPERS_H
#include <string>
#include <vector>

std::string toLowerCase(std::string word);
std::string trim(const std::string &str);
int parseInt(const std::string& arg , const std::string& argName);
bool isValidName(const std::string& name);
std::string combineString(const std::vector<std::string>& args,const int& start);
#endif //RMS_HELPERS_H
