// Created by Omar on 12/1/2025.

#ifndef RMS_RMSCOMMAND_H
#define RMS_RMSCOMMAND_H

#include <string>
#include "structures/unordered_map.h"

using std::string;
class RMSCommand
{
public:
    // enums to select the needed command
    enum class MainCmd
    {
        TRAIN,
        PASSENGER,
        TICKET,
        SYSTEM,
        UNKNOWN
    };
    enum class TrainCmd
    {
        LIST,
        ADD,
        DELETE,
        UPDATE,
        SEATS_ADD,
        STATUS,
        AVAILABILITY,
        GET,
        UNKNOWN
    };
    enum class PassengerCmd
    {
        LIST,
        ADD,
        DELETE,
        UPDATE,
        GET,
        UNKNOWN
    };
    enum class TicketCmd
    {
        LIST,
        BOOK,
        CANCEL,
        GET,
        UNKNOWN
    };
    enum class SystemCmd
    {
        HELP,
        EXIT,
        CLEAR,
        UNKNOWN
    };
    // lookup trees - static to use it without making objects
    static MainCmd getMainCmd(const string &cmd);

    static TrainCmd getTrainCmd(const string &sub);

    static PassengerCmd getPassengerCmd(const string &sub);

    static TicketCmd getTicketCmd(const string &sub);

    static SystemCmd getSystemCmd(const string &cmd);
};

#endif // RMS_RMSCOMMAND_H
