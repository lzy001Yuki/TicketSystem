cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 17)


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
)
