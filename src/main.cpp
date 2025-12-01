#include "models/SeatAllocator.h"
#include <iostream>
#include "CLIController.h"
#include "StartupManager.h"

void loadMockData(RMSFacade* facade) {
    // ---- Add Trains ----
    facade->addTrain("Alex NightLine", 30);
    facade->addTrain("Luxor Premium", 40);

    // ---- Add Passengers ----
    facade->addPassenger("Omar");
    facade->addPassenger("Sara");
    facade->addPassenger("Mohamed");
    facade->addPassenger("Nour");

    // ---- Book Tickets ----
    facade->bookTicket(1, "Omar");
    facade->bookTicket(2, "Mohamed");
    facade->bookTicket(1, "Sara");
}

int main(int argc, char **argv)
{
    StartupManager* startupManager = new StartupManager();
    auto facade =startupManager->buildFacade();
    loadMockData(facade);

    auto cli = CLIController(facade);
    cli.run();
    return 0;
}