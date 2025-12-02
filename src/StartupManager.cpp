//
// Created by Omar on 11/25/2025.
//
#include "StartupManager.h"
#include "Repo/InMemoryTrainRepository.h"
#include "Repo/InMemoryTicketRepository.h"
#include "Repo/InMemoryPassengerRepository.h"
void loadMockData(RMSFacade* facade) {
    // ---- Add Trains ----
    facade->addTrain("Alex NightLine", 30);
    facade->addTrain("Luxor Premium", 40);
    facade->addTrain("Cairo Express", 25);
    facade->addTrain("Aswan Sleeper", 20);
    facade->addTrain("Mediterranean Star", 35);

    // ---- Add Passengers ----
    facade->addPassenger("Omar");
    facade->addPassenger("Sara");
    facade->addPassenger("Mohamed");
    facade->addPassenger("Nour");
    facade->addPassenger("Ali");
    facade->addPassenger("Mona");
    facade->addPassenger("Youssef");
    facade->addPassenger("Laila");
    facade->addPassenger("Karim");
    facade->addPassenger("Dina");

    // ---- Book Tickets ----
    facade->bookTicket(1, "Omar");
    facade->bookTicket(1, "Sara");
    facade->bookTicket(1, "Ali");
    facade->bookTicket(2, "Mohamed");
    facade->bookTicket(2, "Nour");
    facade->bookTicket(3, "Mona");
    facade->bookTicket(3, "Youssef");
    facade->bookTicket(4, "Karim");
    facade->bookTicket(4, "Laila");
    facade->bookTicket(5, "Dina");
    facade->bookTicket(5, "Omar");
}

RMSFacade *StartupManager::buildFacade() {
    // all services and repos only deleted when startup manger deleted (unique)

    // build repos
    // liskov principle
    this->trainRepository= std::make_unique<InMemoryTrainRepository>();
    this->ticketRepository= std::make_unique<InMemoryTicketRepository>();
    this->passengerRepository= std::make_unique<InMemoryPassengerRepository>();

    // build services
    //dependancy injection  + giving access (only not the ownership) to the services
    this->trainService = std::make_unique<TrainService>(trainRepository.get());
    this->passengerService = std::make_unique<PassengerService>(passengerRepository.get());
    this->ticketService = std::make_unique<TicketService>(ticketRepository.get(),trainService.get(),passengerService.get());

    // build facade + dependancy injection
    // give facade access to the services
    this->facade = std::make_unique<RMSFacade>(trainService.get(),ticketService.get(),passengerService.get());
    loadMockData(facade.get());

    // allow acces to facade
    return facade.get();
}
