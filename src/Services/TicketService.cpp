#include "Services/TicketService.h"
#include <stdexcept> //for run time exception
#include <iostream>

TicketService::TicketService(ITicketRepository *repo, TrainService *ts, PassengerService *ps):ticketRepository(repo),trainService(ts),passengerService(ps) {

}


std::optional<Ticket> TicketService::getTicket(const int& ticketId)
{
    return ticketRepository->getTicketById(ticketId);
}

vector<Ticket> TicketService::getAllTickets()
{
    return ticketRepository->getAllTickets();
}

std::optional<Ticket> TicketService::getTicketByTrainAndPassenger(const int& trainId, const int& passengerId)
{

    return ticketRepository->getTicketByTrainAndPassenger(trainId, passengerId);
}

std::optional<Ticket> TicketService::bookTicket(const int& trainId, const int& passengerId)
{

    // 1) get train by id if exist
    auto train = trainService->getTrain(trainId);
    if(!train.has_value()){// not found
        throw std::runtime_error("train with id : " +  std::to_string(trainId) + "does not exit");
    }

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
    int seat_number=train->getSeatAllocator()->allocateSeat(passengerId);
    trainService->save(train.value());
    if(seat_number == -1) // added to waiting list
        return std::nullopt;
    // 5)  create ticket if available
    Ticket t(0,seat_number,trainId , passenger.value());
    ticketRepository->save(t);
    return t;
}

void TicketService::cancelTicket(const int& ticketId)
{

    // get train
    auto ticket =ticketRepository->getTicketById(ticketId);
    if(!ticket.has_value()){ //not found
        throw std::runtime_error("ticket with id : " +  std::to_string(ticketId) + " does not exit");
    }

    // return error if it is already cancelled
    if(ticket->getStatus() == cancelled){
        throw std::runtime_error("ticket with id : " +  std::to_string(ticketId) + " is already cancelled");
    }

    //find train
    auto train = trainService->getTrain(ticket->getTrainId());
    if(!train.has_value() || train->getSeatAllocator() == nullptr){
        throw std::runtime_error("train is not exist or not has seat allocator");
    }

    // pass seat to waiting list
    auto waitingPassengerId  = train->getSeatAllocator()->freeSeat(ticket->getSeat());
    if(waitingPassengerId  == -1)
        throw std::runtime_error("fail to free the seat \n");
    trainService->save(train.value());
    // book seat to another passenger from waiting list if available
    if (waitingPassengerId > 0)   // >0 means there was a waiting passenger
        try {
            auto t = bookTicket(train->getTrainId(), waitingPassengerId);
            if(t.has_value())
                t->print("======Ticket booked to passenger " + std::to_string(waitingPassengerId) + " Successfully ========== \n" );
        } catch (const std::exception &e) {
            // Log error and continue with cancellation
            std::cerr << "Failed to book ticket for waiting passenger "
                      << waitingPassengerId << ": " << e.what() << "\n";
        }


    // cancel ticket
    ticket->setStatus(cancelled);
    ticketRepository->save(ticket.value());
}

Ticket TicketService::updateTicket(Ticket &t) {
    ticketRepository->save(t);
    return t;

}

