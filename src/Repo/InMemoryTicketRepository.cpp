#include "Repo/InMemoryTicketRepository.h"
#include <stdexcept>
#include <iostream>

std::optional<Ticket> InMemoryTicketRepository::getTicketByTrainAndPassenger(int trainId, int passengerId)
{
    for (const auto &p : tickets)
    {
        const Ticket &t = p.second;
        if (t.getTrainId() == trainId && t.getPassenger().getId() == passengerId)
        {
            return t;
        }
    }
    return std::nullopt;//not found
}

bool InMemoryTicketRepository::deleteTicket(int ticketId)
{
    auto it = tickets.find(ticketId);
    if (it != tickets.end())
    {
        tickets.erase(it);
        return true;
    }
    return false;
}

void InMemoryTicketRepository::save( Ticket& ticket)
{
    // later
    if (ticket.getId() == 0)
    {
        ticket.setId(next_id++);
    }
    else if (ticket.getId() >= next_id)
    {
        next_id = ticket.getId() + 1;
    }

    int id = ticket.getId();
    auto result = tickets.emplace(id, ticket);
    if (!result.second)
    {
        result.first->second = ticket; // update existing
    }

}

std::vector<Ticket> InMemoryTicketRepository::getAllTickets()
{
    std::vector<Ticket> results;
    for (const auto &p : tickets)
    {
        results.push_back(p.second);
    }
    return results;
}

std::optional<Ticket> InMemoryTicketRepository::getTicketById(int ticketId)
{
    auto it = tickets.find(ticketId);
    if (it != tickets.end())
    {
        return it->second;
    }
    throw std::runtime_error("Ticket not found");
}

void InMemoryTicketRepository::clear()
{
    tickets.clear();
    next_id= 1;
    std::cout << "All tickets destroyed\n";

}
