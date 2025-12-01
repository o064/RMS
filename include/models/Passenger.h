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
    Passenger(const int id, const std::string& name);
    int getId() const;
    std::string getName() const;
    void setName(const std::string& name);

    void setId(const int & passengerId) ;

};
#endif //RMS_PASSENGER_H
