#ifndef TICKETSYSTEM_ERROR_HPP
#define TICKETSYSTEM_ERROR_HPP

#include <exception>
#include <string>

class InvalidExp : public std::exception{
public:
    const char* what() const noexcept override{
        return "Invalid\n";
    }
};
#endif //TICKETSYSTEM_ERROR_HPP
