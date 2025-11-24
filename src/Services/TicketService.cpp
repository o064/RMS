#include "Services/TicketService.h"

TicketService::TicketService(ITicketRepository *repo)
{
    this->ticketRepository = repo;
}

Ticket TicketService::getTicket(int ticketId)
{
    return ticketRepository->getTicketById(ticketId);
}

std::vector<Ticket> TicketService::getAllTickets()
{
    return ticketRepository->getAllTickets();
}

Ticket TicketService::getTicketByTrainAndPassenger(int trainId, int passengerId)
{
    return ticketRepository->getTicketByTrainAndPassenger(trainId, passengerId);
}

Ticket TicketService::bookTicket(int trainId, int passengerId)
{
    // later
    Passenger p(passengerId, "");
    Ticket t(0, 0, trainId, p);
    return ticketRepository->save(t);
}

bool TicketService::cancelTicket(int ticketId)
{
    Ticket t = ticketRepository->getTicketById(ticketId);
    t.setStatus(cancelled);
    ticketRepository->save(t);
    return true;
}
