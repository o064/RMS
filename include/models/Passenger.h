//
// Created by Omar on 11/19/2025.
//

#ifndef RMS_PASSENGER_H
#define RMS_PASSENGER_H
#include <string>
class Passenger{
    std::string name;
    int id;
public:
    Passenger(int id, std::string name);

    int getId() const;
    std::string getName() const;
};
#endif //RMS_PASSENGER_H
