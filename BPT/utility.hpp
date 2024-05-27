#ifndef TICKETSYSTEM_UTILITY_HPP
#define TICKETSYSTEM_UTILITY_HPP
#include<utility>
#include<iostream>
#include<cstring>
namespace Yuki {
    template <class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;

        constexpr pair() : first(), second() {}

        pair(const pair &other) = default;

        pair(pair &&other) = default;

        pair(const T1 &x, const T2 &y) : first(x), second(y) {}

        template <class U1, class U2>
        pair(U1 &&x, U2 &&y) : first(x), second(y) {}

        template <class U1, class U2>
        pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}

        template <class U1, class U2>
        pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}

        pair& operator=(const pair& other) {
            if (this == &other) return *this;
            first = other.first;
            second = other.second;
            return *this;
        }

        bool operator<(const pair &other) {
            if (first > other.first) return false;
            if (first < other.first) return true;
            if (first == other.first) {
                if (second < other.second) return true;
                else return false;
            }
        }

        bool operator==(const pair &other) {
            return first == other.first && second == other.second;
        }

        bool operator>(const pair &other) {
            return !(*this == other || *this < other);
        }

        bool operator!=(const pair &other) {
            return !(*this == other);
        }

        bool operator >= (const pair &other) {
            return (*this == other) || (*this > other);
        }

        bool operator <= (const pair &other) {
            return (*this < other) || (*this == other);
        }
    };
/*
    template<class T2>
    class pair<char, T2>{
    public:
        char first[30] = {'\0'};
        T2 second;
        pair() = default;
        explicit pair(const char key[], const T2 &value):second(value) {
            strcpy(first, key);
        }
        pair(const pair& other) {
            strcpy(first, other.first);
            second = other.second;
        }
        pair &operator=(const pair& other) {
            if (this == &other) return *this;
            strcpy(first, other.first);
            second = other.second;
            return *this;
        }

        bool operator<(const pair& other) {
            if (strcmp(first, other.first) < 0) return true;
            if (strcmp(first, other.first) > 0) return false;
            if (second < other.second) return true;
            else return false;
        }

        bool operator==(const pair& other) {
            if (strcmp(first, other.first) == 0 && second == other.second) return true;
            else return false;
        }

        bool operator != (const pair& other) {
            if (strcmp(first, other.first) == 0 && second == other.second) return false;
            else return true;
        }

        bool operator> (const pair& other) {
            return !((*this == other) || (*this < other));
        }

        bool operator <= (const pair& other) {
            return !(*this > other);
        }

        bool operator >= (const pair& other) {
            return !(*this < other);
        }
    };

    template<>
    class pair<char, char>{
    public:
        char first[66] = {'\0'};
        char second[66] = {'\0'};
        pair() = default;
        explicit pair(const char key[], const char value[]) {
            strcpy(first, key);
            strcpy(second, value);
        }
        pair(const pair& other) {
            strcpy(first, other.first);
            strcpy(second, other.second);
        }
        pair &operator=(const pair& other) {
            if (this == &other) return *this;
            strcpy(first, other.first);
            strcpy(second, other.second);
            return *this;
        }

        bool operator<(const pair& other) {
            if (strcmp(first, other.first) < 0) return true;
            if (strcmp(first, other.first) > 0) return false;
            if (strcmp(second, other.second) < 0) return true;
            else return false;
        }

        bool operator==(const pair& other) {
            if (strcmp(first, other.first) == 0 && strcmp(second, other.second) == 0) return true;
            else return false;
        }

        bool operator != (const pair& other) {
            if (strcmp(first, other.first) == 0 && strcmp(second, other.second) == 0) return false;
            else return true;
        }

        bool operator> (const pair& other) {
            return !((*this == other) || (*this < other));
        }

        bool operator <= (const pair& other) {
            return !(*this > other);
        }

        bool operator >= (const pair& other) {
            return !(*this < other);
        }
    };
    */
}
#endif //TICKETSYSTEM_UTILITY_HPP