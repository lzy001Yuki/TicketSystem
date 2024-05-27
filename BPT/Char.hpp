#ifndef TICKETSYSTEM_CHAR_HPP
#define TICKETSYSTEM_CHAR_HPP
#include<cstring>
#include<iomanip>
template<int len>
class myChar{
private:
    char Char[len] = {'\0'};
public:
    myChar(){
        Char[len] = {'\0'};
    }
    explicit myChar(const char* c) {
        strcpy(Char, c);
    }
    explicit myChar(const std::string& s) {
        strcpy(Char, s.c_str());
    }
    myChar(const myChar& other) {
        strcpy(Char, other.Char);
    }
    myChar& operator=(const myChar& other) {
        if (this == &other) return *this;
        strcpy(Char, other.Char);
        return *this;
    }
    bool operator==(const myChar& other) const{
        return strcmp(Char, other.Char) == 0;
    }
    bool operator<(const myChar& other) const{
        return strcmp(Char, other.Char) < 0;
    }
    bool operator > (const myChar& other) const{
        return strcmp(Char, other.Char) > 0;
    }
    bool operator != (const myChar& other) const {
        return !(*this == other);
    }
    bool operator >= (const myChar& other) const {
        return !(*this < other);
    }
    bool operator <= (const myChar& other) const {
        return !(*this > other);
    }
    void toChar(const std::string &str) {
        strcpy(Char, str.c_str());
    }
    bool isNull() const {
        return Char[0] == '\0';
    }
    friend std::ostream& operator<<(std::ostream& os, const myChar& obj) {
        os<<obj.Char;
        return os;
    }
};

#endif //TICKETSYSTEM_CHAR_HPP
