

#include "utils/helpers.h"
#include <cctype> // for toLower
#include <stdexcept>
#include <regex>

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
std::string combineString(const std::vector<std::string>& args,const int& start){
    std::string text;
    for(int i = start ; i < args.size() ; i++){
        if(!isValidName(args[i]))
            throw std::runtime_error("invalid name");
        text+= args[i] + " ";
    }
    text.pop_back(); //remove last char
    return text;
}