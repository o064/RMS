//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"


std::list<Train> RMSFacade::listTrains() {
    return trainService->getAllTrains();
}

RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps) {
    this->trainService= ts;
    this->ticketService=tks;
    this->passengerService=ps;
}

Train RMSFacade::addTrain(const std::string& name,const int& totalSeats) {
    return trainService->createTrain(name,totalSeats);
}

