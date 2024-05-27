cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

add_executable(code
        main.cpp
        BPT/BPTree.hpp
        BPT/FileSystem.hpp
        BPT/vector.hpp
        BPT/queue.hpp
        BPT/utility.hpp
        BPT/HashMap.hpp
        BPT/priority_queue.hpp
        src/error.hpp
        src/TicketSystem.hpp
        src/TrainSystem.hpp
        src/UserSystem.hpp
        src/TokenScanner.hpp
        BPT/map.hpp
        BPT/exceptions.hpp
        src/timeManagement.hpp
        src/Compare.hpp
        src/trainType.hpp
        src/ticketType.hpp
        src/userType.hpp
        BPT/Char.hpp
)
