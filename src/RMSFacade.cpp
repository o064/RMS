//
// Created by Omar on 11/21/2025.
//
#include "RMSFacade.h"


std::vector<std::shared_ptr<Train>> RMSFacade::listTrains() {
    return trainService->getAllTrains();
}

RMSFacade::RMSFacade(TrainService *ts, TicketService *tks, PassengerService *ps) {
    this->trainService= ts;
    this->ticketService=tks;
    this->passengerService=ps;
}

std::shared_ptr<Train> RMSFacade::addTrain(const std::string& name,const int& totalSeats) {
    return trainService->createTrain(name,totalSeats);
}

std::shared_ptr<Train> RMSFacade::getTrain(const int &trainId) {
    return trainService->getTrain(trainId);
}
