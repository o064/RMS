

#include "utils/helpers.h"
#include <cctype> // for toLower

std::string toLowerCase(std::string word){
    for(char & ch : word){
        ch = (char)tolower(ch);
    }
    return word;
}