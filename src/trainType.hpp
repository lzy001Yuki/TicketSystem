#ifndef TICKETSYSTEM_TRAINTYPE_HPP
#define TICKETSYSTEM_TRAINTYPE_HPP
#include<cstring>
#include<string>
#include"../BPT/vector.hpp"
#include"../BPT/BPTree.hpp"
#include"../BPT/priority_queue.hpp"
#include "timeManagement.hpp"
const int trainIdMax = 24;
const int staIdMax = 30;
const int staNumMax = 25;
const int durMax = 100; // 发车的最长时长
class StationInfo{
    friend class TrainSystem;
    friend class TrainInfo;
    friend class TicketSystem;
private:
    char name[staIdMax] = {'\0'};
    int price = 0; // 第一站的票价为0
    int arriveTime = 0;
    int leaveTime = 0;
public:
    StationInfo() = default;
    explicit StationInfo(const char *n, int p, int at, int lt) :price(p), arriveTime(at), leaveTime(lt){
        strcpy(name, n);
    }
    StationInfo &operator=(const StationInfo &other) {
        if (this == &other) return *this;
        strcpy(name, other.name);
        price = other.price;
        arriveTime = other.arriveTime;
        leaveTime = other.leaveTime;
        return *this;
    }
    bool operator< (const StationInfo& other) const {
        if (strcmp(name, other.name) < 0) return true;
        else return false;
    }
    bool operator == (const StationInfo& other) const {
        if (strcmp(name, other.name) == 0) return true;
        else return false;
    }
    bool operator> (const StationInfo& other) const {
        if (strcmp(name, other.name) > 0) return true;
        else return false;
    }
    bool operator != (const StationInfo& other) {
        if (strcmp(name, other.name) != 0) return true;
        else return false;
    }
};
class Seats{
public:
    int seat[staNumMax + 1] = {0};
    Seats& operator=(const Seats& other) {
        if (this == &other) return *this;
        for (int i = 0; i <= staNumMax; i++) seat[i] = other.seat[i];
        return *this;
    }
};
class TrainInfo{
    friend class TrainSystem;
    friend class timeComp;
    friend class costComp;
    friend class TicketSystem;
    friend class TransComp;
private:
    char trainID[24] = {'\0'};
    int stationNum = 0;
    char type = '\0';
    Time ini_time;
    // 1-based
    StationInfo stations[22];
    Date date;
    bool isRelease = false;
    int maxSeats = 0;
    int index = 0;
public:
    TrainInfo(){}
    explicit TrainInfo(const char* id, const int &n, char t, const Date &date_, const Time &time, bool flag, const int &ms) :stationNum(n), type(t),date(date_), ini_time(time), isRelease(flag), maxSeats(ms){
        strcpy(trainID, id);
    }
    explicit TrainInfo(const TrainInfo& other) {
        strcpy(trainID, other.trainID);
        stationNum = other.stationNum;
        type = other.type;
        ini_time = other.ini_time;
        date = other.date;
        isRelease = other.isRelease;
        for (int i = 1; i <= stationNum; i++) {
            stations[i] = other.stations[i];
        }
        index = other.index;
        maxSeats = other.maxSeats;
    }

    TrainInfo& operator=(const TrainInfo& other) {
        if (this == &other) return *this;
        strcpy(trainID, other.trainID);
        stationNum = other.stationNum;
        type = other.type;
        ini_time = other.ini_time;
        date = other.date;
        isRelease = other.isRelease;
        index = other.index;
        maxSeats = other.maxSeats;
        for (int i = 1; i <= stationNum; i++) {
            stations[i] = other.stations[i];
        }
        return *this;
    }
    bool operator==(const TrainInfo& other) {
        if (strcmp(trainID, other.trainID) == 0) return true;
        else return false;
    }
    bool operator < (const TrainInfo& other) {
        if (strcmp(trainID, other.trainID) < 0) return true;
        else return false;
    }
    bool operator > (const TrainInfo& other) {
        if (strcmp(trainID, other.trainID) > 0) return true;
        else return false;
    }
};
class basicInfo {
    friend class TrainSystem;
    friend class TrainFunction;
    friend class TicketSystem;
    friend class TrainSta;
private:
    //char tr_id[24] = {'\0'};
    Date date;
    Time ini;
    int index = 0; // train在file中的index
    int maxSeats = 0;
public:
    basicInfo() = default;
    explicit basicInfo(const Date &d, const Time &t, int &i, int &ms) :date(d), ini(t), index(i), maxSeats(ms) {
        //strcpy(tr_id, id);
    }
    basicInfo(const basicInfo& obj) {
        //strcpy(tr_id, obj.tr_id);
        date = obj.date;
        ini = obj.ini;
        index = obj.index;
        maxSeats = obj.maxSeats;
    }
    basicInfo& operator=(const basicInfo& obj) {
        if (this == &obj) return *this;
        //strcpy(tr_id, obj.tr_id);
        date = obj.date;
        ini = obj.ini;
        index = obj.index;
        maxSeats = obj.maxSeats;
        return *this;
    }
    bool operator==(const basicInfo& other) const {
        //return strcmp(tr_id, other.tr_id) == 0;
        return index == other.index;
    }
    bool operator<(const basicInfo& other) const {
        //return strcmp(tr_id, other.tr_id) < 0;
        return index < other.index;
    }
};
class TrainSta{
    friend class TrainSystem;
    friend class timeComp;
    friend class costComp;
    friend class TransComp;
private:
    basicInfo info;
    char trainID[24] = {'\0'};
    int st_index = 0;// 这一站在train中是第几站
    int price = 0;
    int a_time = 0;
    int l_time = 0;
public:
    TrainSta() = default;
    explicit TrainSta(const char *i, basicInfo &bi, int &si, int &p, int &at, int &lt) : st_index(si), price(p), a_time(at), l_time(lt){
        info = bi;
        strcpy(trainID, i);
    }
    TrainSta& operator=(const TrainSta& other) {
        if (this == &other) return *this;
        info = other.info;
        a_time = other.a_time;
        l_time = other.l_time;
        price = other.price;
        st_index = other.st_index;
        strcpy(trainID, other.trainID);
        return *this;
    }
    bool operator ==(const TrainSta& other) const {
        return info.index == other.info.index;
    }
    bool operator < (const TrainSta& other) const {
        return info.index < other.info.index;
    }
};
class compInfo{
    friend class timeComp;
    friend class costComp;
    friend class TrainSystem;
    friend class TransComp;
private:
    int price = 0;
    int time = 0;
    int st_lt = 0;
    int en_at = 0;
    basicInfo info;
    char trainID[24] = {'\0'};
    int st = 0;
    int en = 0;
public:
    compInfo() = default;
    explicit compInfo(const char *i, int p, int t, basicInfo& bi, int lt, int at, int si, int ei) :price(p), time(t), st_lt(lt), en_at(at), st(si), en(ei){
        info = bi;
        strcpy(trainID, i);
    }
    compInfo& operator=(const compInfo& other) {
        if (this == &other) return *this;
        price = other.price;
        time = other.time;
        info = other.info;
        strcpy(trainID, other.trainID);
        st_lt = other.st_lt;
        en_at = other.en_at;
        st = other.st;
        en = other.en;
        return *this;
    }
};
class timeComp{
public:
    bool operator() (const compInfo& a, const compInfo& b) const {
        if (a.time > b.time) return true;
        else if (a.time == b.time) {
            if (strcmp(a.trainID, b.trainID) > 0) return true;
            else return false;
        } else return false;
    }
};
class costComp{
public:
    bool operator() (const compInfo& a, const compInfo& b) const {
        if (a.price > b.price) return true;
        else if (a.price == b.price) {
            if (strcmp(a.trainID, b.trainID) > 0) return true;
            else return false;
        } else return false;
    }
};
class TrainFunction{
public:
    int operator() (const int &num) {
        return num;
    }
    int operator() (const basicInfo& b) {
        return b.index;
    }
};
class TransComp{
    friend class TrainSystem;
private:
    int time = 0;
    int cost = 0;
    compInfo first;
    compInfo second;
    Yuki::pair<Day, Time> mid_arr;
    Yuki::pair<Day, Time> mid_lea;
    Yuki::pair<Day, Time> st_lea;
    Yuki::pair<Day, Time> en_arr;
    char mid[30] = {'\0'};
public:
    TransComp() = default;
    explicit TransComp(int t, int c, const char *i) : time(t), cost(c) {
        strcpy(mid, i);
    }
    TransComp& operator=(const TransComp& other) {
        if (this == &other) return *this;
        time = other.time;
        cost = other.cost;
        first = other.first;
        second = other.second;
        mid_arr = other.mid_arr;
        mid_lea = other.mid_lea;
        st_lea = other.st_lea;
        en_arr = other.en_arr;
        strcpy(mid, other.mid);
        return *this;
    }
    bool comp_cost(const TransComp& other) {
        if (cost > other.cost) return true;
        if (cost == other.cost) {
            if (time > other.time) return true;
            else if (time < other.time) return false;
            else {
                if (strcmp(first.trainID, other.first.trainID) > 0) return true;
                else if (strcmp(first.trainID, other.first.trainID) < 0) return false;
                else {
                    if (strcmp(second.trainID, other.second.trainID) > 0) return true;
                    else return false;
                }
            }
        } else return false;
    }
    bool comp_time(const TransComp& other) {
        if (time > other.time) return true;
        if (time == other.time) {
            if (cost > other.cost) return true;
            else if (cost < other.cost) return false;
            else {
                if (strcmp(first.trainID, other.first.trainID) > 0) return true;
                else if (strcmp(first.trainID, other.first.trainID) < 0) return false;
                else {
                    if (strcmp(second.trainID, other.second.trainID) > 0) return true;
                    else return false;
                }
            }
        } else return false;
    }
};

#endif //TICKETSYSTEM_TRAINTYPE_HPP
