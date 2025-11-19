cmake_minimum_required(VERSION 3.13) # CMake version check
project(RMS) # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14) # Enable C++14 standard
# Add main.cpp file of the project root directory as a source file
set(src main.cpp)
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(RMS ./src/main.cpp
        include/models/Train.h
        include/models/SeatAllocator.h
        include/models/Ticket.h
        include/Repo/ITrainReposatory.h
        include/Repo/InMemoryTrainReposatory.h
        include/Services/TrainService.h
        include/Services/TrainService.h
        include/models/Passenger.h
        include/Services/PassengerService.h
        include/Services/TicketService.h
        "include/Repo/ITicketRepository.h"
        "include/Repo/InMemoryTicketRepository .h"
        include/Repo/IPassengerRepository.h
        "include/Repo/InMemoryPassengerRepository.h"
        include/RMSFacade.h
        include/StartupManager.h
        include/CLIController.h
)