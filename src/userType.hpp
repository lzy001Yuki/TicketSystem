#ifndef TICKETSYSTEM_USERTYPE_HPP
#define TICKETSYSTEM_USERTYPE_HPP
#include<cstring>
#include<iomanip>
class UserManagement;
class UserInfo{
    friend class UserManagement;
    friend class TicketSystem;
private:
    myChar<24> username;
    myChar<32> password;
    myChar<24> name;
    myChar<32> mailAddr;
    int privilege = -1;
public:
    UserInfo() = default;
    explicit UserInfo(const myChar<24>& un, const myChar<32>& pw, const myChar<24>& n, const myChar<32>& ma, int p, bool flag = false) :privilege(p)
    {
        username = un;
        password = pw;
        name = n;
        mailAddr = ma;
    }
    friend std::ostream &operator<<(std::ostream &os, const UserInfo &user_info) {
        os<<user_info.username<<' '<<user_info.name<<' '<<user_info.mailAddr<<' '<<user_info.privilege<<'\n';
        return os;
    }

    bool operator == (const UserInfo& other) {
        return username == other.username;
    }

    bool operator < (const UserInfo& other) {
        if (username < other.username) return true;
        else return false;
    }

    bool operator > (const UserInfo &other) {
        if (username > other.username) return true;
        else return false;
    }

    UserInfo& operator=(const UserInfo & other) {
        if (this == &other) return *this;
        username = other.username;
        password = other.password;
        name = other.name;
        mailAddr = other.mailAddr;
        privilege = other.privilege;
        return *this;
    }
    ~UserInfo() = default;
};
class nameFunction{
public:
    int operator() (const int &num) {
        return num * 13;
    }
};
#endif //TICKETSYSTEM_USERTYPE_HPP
