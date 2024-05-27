#ifndef TICKETSYSTEM_TICKETTYPE_HPP
#define TICKETSYSTEM_TICKETTYPE_HPP
#include"../BPT/BPTree.hpp"
#include"TrainSystem.hpp"
#include"UserSystem.hpp"
#include"trainType.hpp"

// 维护一个TrainTime-Ticket的BPT 这里Ticket是waitingList里面的，顺序按照时间顺序
// 维护一个User-Ticket的BPT 是用户成功购买的订单，候补也算

enum Status{success, pending, refunded};

class Ticket{
    friend class TicketSystem;
private:
    myChar<24> trainID;
    myChar<30> st_n;
    myChar<30> en_n;
    int st = 0;
    int en = 0;
    int num = 0;
    Day day;// 该车次的始发站日期
    Time ini;
    int st_t = 0;
    int en_t = 0;
    Status status = success;
    int time = 0;
    int price = 0;
public:
    Ticket() = default;
    explicit Ticket(const myChar<24>& tr, const myChar<30>& sn, const myChar<30>& enn, int s, int e, int n, const Day& d, int time_, Status sta
            ,int stt, int ent, int p, Time &in) :num(n), status(sta), day(d), time(time_), st(s), en(e), ini(in),
            st_t(stt), en_t(ent), price(p){
        trainID = tr;
        st_n = sn;
        en_n = enn;
    }
    bool operator<(const Ticket& other) const {
        return time < other.time;
    }
    bool operator == (const Ticket& other) const {
        return time == other.time;
    }
    bool operator != (const Ticket& other) const {
        return time != other.time;
    }
    Ticket& operator= (const Ticket & other) {
        if (this == &other) return *this;
        num = other.num;
        time = other.time;
        day = other.day;
        status = other.status;
        trainID = other.trainID;
        st_n = other.st_n;
        en_n = other.en_n;
        st_t = other.st_t;
        en_t = other.en_t;
        st = other.st;
        en = other.en;
        price = other.price;
        ini = other.ini;
        return *this;
    }
};

class TrainTime{
    friend class TicketSystem;
private:
    myChar<24> trainID;
    Day day;/// 注意应该是发车时间！！！
public:
    TrainTime() = default;
    explicit TrainTime(const myChar<24>& t, const Day &d) :day(d) {
        trainID = t;
    }
    bool operator<(const TrainTime& other) const {
        if (trainID < other.trainID) return true;
        else if (trainID > other.trainID) return false;
        else {
            return day < other.day;
        }
    }
    bool operator == (const TrainTime& other) const {
        return (day == other.day) && (trainID == other.trainID);
    }
    bool operator != (const TrainTime& other) const {
        return !(*this == other);
    }
    bool operator > (const TrainTime &other) const {
        if (*this == other || *this < other) return false;
        else return true;
    }
};
class TicketFunction{
public:
    int operator() (const int &num) {
        return num * 13;
    }
};
class Wait{
    friend class TicketSystem;
private:
    char user[24] = {'\0'};
    int num = 0;
    int st_i = 0;
    int en_i = 0;
    int time = 0;
public:
    Wait() = default;
    explicit Wait(const char* u, int n, int si, int ei, int t) :num(n), st_i(si), en_i(ei), time(t) {
        strcpy(user, u);
    }
    bool operator == (const Wait& other) const {
        return time == other.time;
    }
    bool operator < (const Wait& other) const {
        return time < other.time;
    }
};

#endif //TICKETSYSTEM_TICKETTYPE_HPP
