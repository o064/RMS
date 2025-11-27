#include "Services/TicketService.h"
#include <stdexcept> //for run time exception
TicketService::TicketService(ITicketRepository *repo, TrainService *ts, PassengerService *ps):ticketRepository(repo),trainService(ts),passengerService(ps) {

}


std::optional<Ticket> TicketService::getTicket(const int& ticketId)
{
    return ticketRepository->getTicketById(ticketId);
}

std::vector<Ticket> TicketService::getAllTickets()
{
    return ticketRepository->getAllTickets();
}

std::optional<Ticket> TicketService::getTicketByTrainAndPassenger(const int& trainId, const int& passengerId)
{

    return ticketRepository->getTicketByTrainAndPassenger(trainId, passengerId);
}

Ticket TicketService::bookTicket(const int& trainId, const int& passengerId)
{

    // 1) get train by id if exist
    auto train = trainService->getTrain(trainId);
    if(!train.has_value()){// not found
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");
    }
    if(!train->hasAvailableSeats())
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "is Full");

    // 2) get passenger by id if exist
    auto  passenger = passengerService->getPassenger(passengerId);
    if(!passenger.has_value()){// not found
        throw std::runtime_error("passenger with id : " +  std::to_string(passengerId) + "does not exit");
    }

    // 3) check if passenger has already ticket for this train
    auto  existTicket = ticketRepository->getTicketByTrainAndPassenger(trainId,passengerId);
    if(existTicket.has_value()){//  found
        throw std::runtime_error("cannot allocate more than one ticket for the same passenger in the same train\n");
    }

    // 4) assign seat to passenger if avialble
    int seat_number;
    if(train->getSeatAllocator() != nullptr && train->hasAvailableSeats()){ // if train have seat allocator and seats
        seat_number = train->getSeatAllocator()->allocateSeat(passengerId);
        if(seat_number == -1)
            throw std::runtime_error("failed to assign seat");
        trainService->save(train.value());
    }

    // 5)  create ticket
    Ticket t(0,seat_number,trainId , passenger.value());
    ticketRepository->save(t);
    return t;
}

void TicketService::cancelTicket(const int& ticketId)
{

    // get train
    auto ticket =ticketRepository->getTicketById(ticketId);
    if(!ticket.has_value()){ //not found
        throw std::runtime_error("ticket with id : " +  std::to_string(ticketId) + "does not exit");
    }

    // return error if it is already cancelled
    if(ticket->getStatus() == cancelled){
        throw std::runtime_error("ticket with id : " +  std::to_string(ticketId) + "is already cancelled");
    }

    //find train
    auto train = trainService->getTrain(ticket->getTrainId());
    if(!train.has_value() || train->getSeatAllocator() == nullptr){
        throw std::runtime_error("train is not exist or not has seat allocator");
    }

    // free seat & save train
    train->getSeatAllocator()->freeSeat(ticket->getSeat());
    trainService->save(train.value());
    // cancel ticket
    ticket->setStatus(cancelled);

    //save the changes
    ticketRepository->save(ticket.value());
}

