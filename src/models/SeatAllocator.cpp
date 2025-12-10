//
// Created by Omar on 11/20/2025.
//

#include "models/SeatAllocator.h"
#include <iostream>
#include <functional>

SeatAllocator::SeatAllocator(int totalSeats)
{
    if (totalSeats <= 0)
        this->totalSeats = 10;
    else
        this->totalSeats = totalSeats;
    //    add the seats to the set
    for (int seat = 1; seat <= this->totalSeats; seat++)
        availableSeats.insert(seat);
}

int SeatAllocator::allocateSeat(int passengerId)
{
    // prevent duplicate passenger allocation
    for (const auto &p : allocatedSeats)
    {
        if (p.second == passengerId)
            throw std::runtime_error("Passenger " + std::to_string(passengerId) + " already has a seat.\n");
    }

    // prevent duplicate waiting list insertion
    if (waitingSet.count(passengerId))
        throw std::runtime_error("Passenger " + std::to_string(passengerId) + " already in waiting list.\n");

    // No available seats , push to waiting list
    if (!hasAvailableSeats())
    {
        waitingList.push(passengerId);
        waitingSet.insert(passengerId);
        std::cout << "Train full, passenger added to waiting list.\n";
        return -1;
    }

    int seatNumber;

    // prefer reusing cancelled seats first
    if (!cancelledSeats.empty())
    {
        seatNumber = cancelledSeats.top();
        cancelledSeats.pop();
    }
    else
    {
        // smallest seat in the set
        seatNumber = *availableSeats.begin();
        availableSeats.erase(availableSeats.begin());
        std::cout << "seat : " << seatNumber;
    }

    allocatedSeats[seatNumber] = passengerId;
    return seatNumber;
}

int SeatAllocator::freeSeat(int seatNumber)
{
    if (seatNumber <= 0 || seatNumber > totalSeats)
        throw std::invalid_argument("Invalid seat number.\n");

    auto it = allocatedSeats.find(seatNumber);
    if (it == allocatedSeats.end())
        throw std::out_of_range("Invalid seat number.\n");

    // delete from the hash map and add to the stack
    allocatedSeats.erase(seatNumber);
    cancelledSeats.push(seatNumber);

    // assign to waiting passenger if any
    if (!waitingList.empty())
    {
        int firstPassenger = waitingList.front();
        waitingList.pop();
        waitingSet.erase(firstPassenger);
        std::cout << "\n";
        std::cout << "Seat " << seatNumber
                  << " freed and assigned to waiting passenger "
                  << firstPassenger << "\n";

        return firstPassenger;
    }

    return 0; // no waiting passengers
}

void SeatAllocator::addSeats(int seats)
{
    if (seats <= 0)
        throw std::invalid_argument("Seats must be greater than zero.\n");

    int oldTotal = totalSeats;
    totalSeats += seats;

    for (int i = oldTotal + 1; i <= totalSeats; i++)
        availableSeats.insert(i);
}

void SeatAllocator::changeTotalSeats(int newTotalSeats)
{
    if (newTotalSeats < getAllocatedSeatCount())
        throw std::out_of_range("Cannot shrink below allocated count.\n");

    if (newTotalSeats > totalSeats)
    {
        // expand
        for (int i = totalSeats + 1; i <= newTotalSeats; i++)
            availableSeats.insert(i);
    }
    else
    {
        // shrink
        int seatsToRemove = totalSeats - newTotalSeats;

        if (availableSeats.size() < seatsToRemove)
            throw std::runtime_error("Cannot shrink: not enough free seats.\n");

        while (seatsToRemove-- > 0)
        {
            auto it = std::prev(availableSeats.end());
            availableSeats.erase(it);
        }
    }

    totalSeats = newTotalSeats;
}

void SeatAllocator::printStatus() const
{
    std::cout << "\n========== Seat Allocation Status ==========\n";

    std::cout << "Total Seats          : " << totalSeats << "\n";
    std::cout << "Allocated Seat Count : " << getAllocatedSeatCount() << "\n";
    std::cout << "Available Seat Count : " << getAvailableSeatCount() << "\n\n";

    // ---- Allocated Seats ----
    std::cout << "--- Allocated Seats (Seat -> Passenger ID) ---\n";
    if (allocatedSeats.empty())
    {
        std::cout << "No seats allocated.\n";
    }
    else
    {
        for (const auto &p : allocatedSeats)
        {
            if (p.second != -1)
                std::cout << "Seat " << p.first << " -> Passenger " << p.second << "\n";
        }
    }
    std::cout << "\n";

    // ---- Available Seats ----
    std::cout << "--- Available Seats ---\n";
    if (availableSeats.empty())
    {
        std::cout << "No free seats.\n";
    }
    else
    {
        for (int s : availableSeats)
            std::cout << s << " ";
        std::cout << "\n";
    }
    std::cout << "\n";

    // ---- Cancelled Seats Stack ----
    std::cout << "--- Cancelled Seats Stack (top to bottom) ---\n";
    if (cancelledSeats.empty())
    {
        std::cout << "Empty\n";
    }
    else
    {
        stack<int> temp = cancelledSeats;
        while (!temp.empty())
        {
            std::cout << temp.top() << " ";
            temp.pop();
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // ---- Waiting List ----
    std::cout << "--- Waiting List (front -> back) ---\n";
    if (waitingList.empty())
    {
        std::cout << "No passengers in waiting list.\n";
    }
    else
    {
        std::queue<int> temp = waitingList;
        while (!temp.empty())
        {
            std::cout << temp.front() << " ";
            temp.pop();
        }
        std::cout << "\n";
    }

    std::cout << "=============================================\n\n";
}

bool SeatAllocator::hasAvailableSeats() const
{
    // if the is seat in cancelled tickets or minHeap
    //   return avaiable(true) if availableSeats or cancelledSeats is not  empty
    return !availableSeats.empty() || !cancelledSeats.empty();
}
int SeatAllocator::getAvailableSeatCount() const
{
    //    return the numbers of seats in the heap and the cancelled seats
    return availableSeats.size() + cancelledSeats.size();
}
std::unique_ptr<SeatAllocator> SeatAllocator::clone() const
{
    return std::make_unique<SeatAllocator>(*this);
}

SeatAllocator::SeatAllocator(const SeatAllocator &other)
    : availableSeats(other.availableSeats),
      waitingList(other.waitingList),
      allocatedSeats(other.allocatedSeats),
      cancelledSeats(other.cancelledSeats),
      waitingSet(other.waitingSet),
      totalSeats(other.totalSeats) {}

SeatAllocator &SeatAllocator::operator=(const SeatAllocator &other)
{
    if (this != &other)
    {
        availableSeats = other.availableSeats;
        waitingList = other.waitingList;
        allocatedSeats = other.allocatedSeats;
        cancelledSeats = other.cancelledSeats;
        waitingSet = other.waitingSet;
        totalSeats = other.totalSeats;
    }
    return *this;
}

int SeatAllocator::getAllocatedSeatCount() const
{
    return allocatedSeats.size();
}

std::queue<int> SeatAllocator::getWaitingList() const
{
    return waitingList;
}

int SeatAllocator::getTotalSeats() const
{
    return totalSeats;
}

int SeatAllocator::getWaitingListSize() const
{
    return waitingList.size();
}

int SeatAllocator::processWaitingList(int seatsToAdd, std::function<void(int)> bookCallback)
{
    int processed = 0;

    std::queue<int> newWaitingList;
    std::set<int> newWaitingset;

    while (!waitingList.empty())
    {
        int passengerId = waitingList.front();
        waitingList.pop();
        waitingSet.erase(passengerId); // remove from set

        if (seatsToAdd > 0)
        {
            try
            {
                bookCallback(passengerId);
                seatsToAdd--;
                processed++;
                continue;                      // do NOT add to new list
            }
            catch (const std::exception &e)
            {
                std::cout << "Failed to book ticket for waiting passenger "
                          << passengerId << ": " << e.what() << "\n";
                // Booking failed , keep in waiting list
            }
        }

        // Either no seats left OR booking failed ,so  keep them
        newWaitingList.push(passengerId);
        newWaitingset.insert(passengerId);
    }

    waitingList = std::move(newWaitingList); //  update the existing waiting list
    waitingSet = std::move(newWaitingset); //  update the existing waiting set

    return processed;
}
