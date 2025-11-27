cmake_minimum_required(VERSION 3.13)

project(RMS)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

# Download and setup Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
        GIT_SHALLOW TRUE
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -------------------------------
# Build your project as a library
# -------------------------------
add_library(rms_lib
        src/models/Passenger.cpp
        src/models/SeatAllocator.cpp
        src/models/Train.cpp
        src/models/Ticket.cpp
        src/Repo/InMemoryTrainRepository.cpp
        src/Repo/InMemoryPassengerRepository.cpp
        src/Repo/InMemoryTicketRepository.cpp
        src/Services/PassengerService.cpp
        src/Services/TicketService.cpp
        src/Services/TrainService.cpp
        src/RMSFacade.cpp
        src/StartupManager.cpp
        src/CLIController.cpp
        src/utils/helpers.cpp

)

# Include directories - adjust based on your actual header locations
target_include_directories(rms_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# -------------------------------
# Build the main executable
# -------------------------------
add_executable(rms_app
        src/main.cpp
)

target_link_libraries(rms_app PRIVATE rms_lib)

# -------------------------------
# Tests
# -------------------------------
add_executable(rms_tests
        tests/test_SeatAllocator.cpp
        tests/test_train.cpp
        tests/test_trainRepo.cpp
        tests/test_trainSevice.cpp
        tests/test_passenger.cpp
        tests/test_passengerRepoMemo.cpp
        tests/test_passengerService.cpp
        tests/test_helpers.cpp
        tests/test_ticket.cpp
        tests/test_ticktRepo.cpp
        tests/test_ticketService.cpp
        tests/test_facade.cpp
        tests/test_startUpManager.cpp
        tests/test_cliController.cpp
        tests/structures/test_stack.cpp
)
target_link_libraries(rms_tests
        PRIVATE
        gtest_main
        gmock
        gmock_main
        rms_lib
)

target_include_directories(rms_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(rms_tests)