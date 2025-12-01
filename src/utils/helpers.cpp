

#include "utils/helpers.h"
#include <cctype> // for toLower
#include <stdexcept>
#include <regex>
#include <iostream>
#include <chrono>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;

std::string toLowerCase(std::string word){
    for(char & ch : word){
        ch = (char)tolower(ch);
    }
    return word;
}
// Function to trim leading and trailing whitespace
std::string trim(const std::string &str) {
    if (str.empty()) return "";

    auto start = 0;
    auto end = str.length() - 1;

    // find first char from the left
    while (start < str.size() && str[start] == ' ') {
        start++;
    }

    // all string is space
    if (start == str.size()) return "";

    // find first char from the right
    while (end > start && str[end] == ' ') {
        end--;
    }

    // return actual string
    return str.substr(start, end - start + 1);
}

int parseInt(const std::string& arg , const std::string& argName){
    try{
        std::size_t pos; // have the index of non int char
       int res = stoi(arg,&pos);
       if(pos == 0 || pos != arg.size()) // to pass 12a and abc
           throw std::invalid_argument("");

       return res;

    }catch (std::invalid_argument& e) {
        throw std::invalid_argument( argName +" must be a number");
    }
    catch ( std::out_of_range& e) {
        throw std::out_of_range( argName +" value is too large");
    }
}

bool isValidName(const std::string& name) {
    static const std::regex pattern("^[A-Za-z]+([' -][A-Za-z]+)*$"); // a-z - '
    return std::regex_match(name, pattern);
}
std::string combineString(const std::vector<std::string>& args,int start){
    if(args[start].empty())
        return "";
    std::string text;
    for(int i = start ; i < args.size() ; i++){
        if(!isValidName(args[i]))
            throw std::runtime_error("invalid name");
        text+= args[i] + " ";
    }
    text.pop_back(); //remove last char
    return text;
}

std::string combineString(const std::vector<std::string>& args, int start , int end){
    if(args[start].empty())
        return "";
    if (end == -1) {
        end = args.size() - 1; // stop at prev-last element
    }

    if (start < 0 || start >= args.size() || end < start) {
        throw std::runtime_error("Invalid start/end range for combineString");
    }
    std::string text;
    for(int i = start ; i < end; i++){
        if(!isValidName(args[i]))
            throw std::runtime_error("invalid name");
        text+= args[i] + " ";
    }
    text.pop_back(); //remove last char
    return text;
}
bool compareString(const std::string& str1 , const std::string& str2){
    return toLowerCase(trim(str1)) ==toLowerCase(trim(str2));
}
bool isInteger(const std::string& str) {
    try {
        std::stoi(str);
        return true;
    } catch (std::exception&) {
        return false;
    }
}

void printTrainInfo(const Train& t, const string& msg){
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Train ID   : " << t.getTrainId() << "\n";
    cout << " Name   : " << t.getTrainName() << "\n";
    cout << "Seats  : " << t.getTotalSeats() << "\n";
    cout << "Availability: " << (t.hasAvailableSeats() ? "Available" : "full") << "\n";
    cout << "--------------------------------------------------\n";
}
void printPassengerInfo(const Passenger& p, const string& msg){
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Passenger ID   : " << p.getId() << "\n";
    cout << "Name   : " << p.getName() << "\n";
    cout << "--------------------------------------------------\n";
}
void printTicketInfo(const Ticket& t, const string& msg){
    cout << "--------------------------------------------------\n";
    cout << msg << endl;
    cout << "Ticket ID: " << t.getId() << "\n";
    cout << "Seat: " << t.getSeat() << "\n";
    cout << "Train ID: " << t.getTrainId() << "\n";
    cout << "Passenger: " << t.getPassenger().getName() << "\n";
    cout << "Status : " << ((t.getStatus() == booked) ? "Booked" : "Cancelled") << "\n";
    cout << "--------------------------------------------------\n";
}

void printCurrentDate(){
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::cout << "Current Date & Time: "
              << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
              << "\n";
}