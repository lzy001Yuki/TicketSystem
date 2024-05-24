#ifndef TICKETSYSTEM_TICKETSYSTEM_HPP
#define TICKETSYSTEM_TICKETSYSTEM_HPP
#include"../BPT/BPTree.hpp"
#include"TrainSystem.hpp"
#include"UserSystem.hpp"

// 维护一个TrainTime-Ticket的BPT 这里Ticket是waitingList里面的，顺序按照时间顺序
// 维护一个User-Ticket的BPT 是用户成功购买的订单，候补也算

enum Status{success, pending, refunded};

class Ticket{
    friend class TicketSystem;
private:
    char user[24] = {'\0'};
    char trainID[24] = {'\0'};
    int st = 0;
    int en = 0;
    int num = 0;
    Day day;// 这张票的始发站日期，但不是该车次的始发站日期
    Status status = success;
    int time = 0;
public:
    Ticket() = default;
    explicit Ticket(const char*u, const char *t, int s, int e, int n, const Day& d, int time_, Status sta) :num(n), status(sta), day(d), time(time_), st(s), en(e) {
        strcpy(user, u);
        strcpy(trainID, t);
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
        strcpy(user, other.user);
        strcpy(trainID, other.trainID);
        st = other.st;
        en = other.en;
    }
};

class TrainTime{
    friend class TicketSystem;
private:
    char trainID[24] = {'\0'};
    Day day;/// 注意应该是发车时间！！！
public:
    TrainTime() = default;
    explicit TrainTime(const char *t, const Day &d) :day(d) {
        strcpy(trainID, t);
    }
    bool operator<(const TrainTime& other) const {
        if (strcmp(trainID, other.trainID) < 0) return true;
        else if (strcmp(trainID, other.trainID) > 0) return false;
        else {
            return day < other.day;
        }
    }
    bool operator == (const TrainTime& other) const {
        return (day == other.day) && (strcmp(trainID, other.trainID) == 0);
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


class TicketSystem{
private:
    BPT<char, Ticket, TicketFunction, 20, 2, 1024> Order;
    BPT<TrainTime, Ticket, TicketFunction, 10, 2, 13> Waiting;

     void update_ticket(TrainInfo &trainInfo, const Ticket& ticket, TrainSystem& train, bool type, Seats& seats) {
        int s_index = ticket.st;
        int t_index = ticket.en;
        if (type) {
            seats.seat[s_index] += ticket.num;
            seats.seat[t_index] -= ticket.num;
        } else {
            seats.seat[s_index] -= ticket.num;
            seats.seat[t_index] += ticket.num;
        }
    }
public:
    TicketSystem(): Order("order.txt", "space_order.txt"), Waiting("waiting.txt", "space_waiting.txt"){}

    void buy_ticket(const char *u, const char *i, Day &d, const int &n, const char *st, const char *en, bool flag, UserManagement& user, TrainSystem& train, int time) {
        auto it = user.LogIn.find(u);
        if (it == user.LogIn.end()) {
            std::cout<<"-1\n";
            return;
        }
        TrainInfo trainInfo;
        basicInfo bi;
        if (!train.trainData.findKV(i, bi)) {
            std::cout<<"-1\n";
            return;
        }
        // 即使release时间也可能不符
        train.trainIndex.read(trainInfo, TrainSystem::indexToPos(bi.index));
        if (!trainInfo.isRelease) {
            std::cout<<"-1\n";
            return;
        }
        int s_index = 1;
        for (; s_index <= trainInfo.stationNum; s_index++) {
            if (strcmp(st, trainInfo.stations[s_index].name) == 0) break;
        }
        if (s_index == trainInfo.stationNum + 1) {
            std::cout<<"-1\n";
            return;
        }
        Day earliest(TrainSystem::showTime(trainInfo.date.st, trainInfo.ini_time, trainInfo.stations[s_index].leaveTime).first);
        Day latest(TrainSystem::showTime(trainInfo.date.en, trainInfo.ini_time, trainInfo.stations[s_index].leaveTime).first);
        Date dur(earliest, latest);
        if (!dur.check(d)) {
            std::cout<<"-1\n";
            return;
        }
        int t_index = s_index + 1;
        for (; t_index <= trainInfo.stationNum; t_index++) {
            if (strcmp(en, trainInfo.stations[t_index].name) == 0) break;
        }
        if (t_index == trainInfo.stationNum + 1) {
            std::cout<<"-1\n";
            return;
        }
        if (t_index <= s_index) {
            std::cout<<"-1\n";
            return;
        }
        // 这辆车的始发日期
        if (n > trainInfo.maxSeats) {
            std::cout<<"-1\n";
            return;
        }
        Day st_day = TrainSystem::checkBegin(d, trainInfo.ini_time, trainInfo.stations[s_index].leaveTime);
        int dur_time = TrainSystem::cal_now(trainInfo.date, st_day);
        Seats s;
        train.seatsIndex.read(s, sizeof(Seats) * (durMax * trainInfo.index + dur_time));
        int seats = train.cal_ticket(s_index, t_index, s, trainInfo.maxSeats);
        if (seats >= n) {
            // 购票成功
            s.seat[s_index] -= n;
            s.seat[t_index] += n;
            int p = n * (trainInfo.stations[t_index].price - trainInfo.stations[s_index].price);
            Ticket ticket(u, i, s_index, t_index, n, d, time, success);
            Order.insert(Yuki::pair<char, Ticket> (u, ticket));
            train.trainIndex.write(trainInfo, TrainSystem::indexToPos(bi.index));
            train.seatsIndex.write(s, sizeof (Seats) * (durMax * trainInfo.index + dur_time));
            std::cout<<p<<'\n';
        } else {
            if (!flag) {
                std::cout<<"-1\n";
                return;
            } else {
                std::cout<<"queue\n";
                Ticket ticket(u, i, s_index, t_index, n, d, time, pending);
                Order.insert(Yuki::pair<char, Ticket> (u, ticket));
                TrainTime trainTime(trainInfo.trainID, st_day);
                Waiting.insert(Yuki::pair<TrainTime, Ticket> (trainTime, ticket));
            }
        }
    }

    void query_order(const char *u, UserManagement& user, TrainSystem& train) {
        UserInfo userInfo;
        int u_index;
        bool exist = user.userData.findKV(u, u_index);
        if (!exist) {
            std::cout<<"-1\n";
            return;
        }
        user.userIndex.read(userInfo, UserManagement::changeToPos(u_index));
        auto it = user.LogIn.find(u);
        if (it == user.LogIn.end()) {
            std::cout<<"-1\n";
            return;
        }
        Yuki::vector<Ticket> log = Order.find(u);
        std::cout<<log.size()<<'\n';
        for (int i = log.size() - 1; i >= 0; i--) {
            if (log[i].status == success) std::cout<<"[success] ";
            if (log[i].status == pending) std::cout<<"[pending] ";
            if (log[i].status == refunded) std::cout<<"[refunded] ";
            std::cout<<log[i].trainID<<" ";
            TrainInfo trainInfo;
            basicInfo bi;
            char tr[66] = {'\0'};
            strcpy(tr, log[i].trainID);
            train.trainData.findKV(log[i].trainID, bi);
            //if (!train.Buffer.find(info_index, trainInfo))
            train.trainIndex.read(trainInfo, TrainSystem::indexToPos(bi.index));
            int s_index = log[i].st;
            Day st_day = TrainSystem::checkBegin(log[i].day, trainInfo.ini_time, trainInfo.stations[s_index].leaveTime);
            Yuki::pair<Day, Time> leave = TrainSystem::showTime(st_day, trainInfo.ini_time, trainInfo.stations[s_index].leaveTime);
            std::cout<<trainInfo.stations[log[i].st].name<<' ';
            std::cout<<leave.first<<' '<<leave.second<<" -> ";
            std::cout<<trainInfo.stations[log[i].en].name<<' ';
            int t_index = log[i].en;
            Yuki::pair<Day, Time> arrive = TrainSystem::showTime(st_day, trainInfo.ini_time, trainInfo.stations[t_index].arriveTime);
            std::cout<<arrive.first<<' '<<arrive.second<<' ';
            int price = trainInfo.stations[t_index].price - trainInfo.stations[s_index].price;
            std::cout<<price<<' '<<log[i].num<<'\n';
        }
    }

    int refund_ticket(const char *u, int n, UserManagement& user, TrainSystem& train) {
        UserInfo userInfo;
        int u_index;
        bool exist = user.userData.findKV(u, u_index);
        if (!exist) return -1;
        user.userIndex.read(userInfo, UserManagement::changeToPos(u_index));
        auto it = user.LogIn.find(u);
        if (it == user.LogIn.end()) return -1;
        Yuki::vector<Ticket> log = Order.find(u);
        if (n > log.size()) return -1;
        Ticket refund = log[log.size() - n];
        TrainInfo trainInfo;
        basicInfo bi;
        train.trainData.findKV(refund.trainID, bi);
        train.trainIndex.read(trainInfo, TrainSystem::indexToPos(bi.index));
        int index = refund.st;
        Day st_day = TrainSystem::checkBegin(refund.day, trainInfo.ini_time, trainInfo.stations[index].leaveTime);
        TrainTime re_train(refund.trainID, st_day);
        if (refund.status == refunded) return -1;
        if (refund.status == pending) {
            Waiting.erase(Yuki::pair<TrainTime, Ticket> (re_train, refund));
        }
        int dur = TrainSystem::cal_now(trainInfo.date, st_day);
        Seats s;
        train.seatsIndex.read(s, sizeof(Seats) * (durMax * trainInfo.index + dur));
        if (refund.status == success) {
            update_ticket(trainInfo, refund, train, true, s);
            Yuki::vector<Ticket> waitingList = Waiting.find(re_train);
            for (int i = 0; i < waitingList.size(); i++) {
                int s_index = waitingList[i].st;
                int t_index = waitingList[i].en;
                // 在re_train.day这一天发车
                int seats = train.cal_ticket(s_index, t_index, s, trainInfo.maxSeats);
                Day w_st = TrainSystem::checkBegin(waitingList[i].day, trainInfo.ini_time,
                                                   trainInfo.stations[s_index].leaveTime);
                if (st_day != w_st) continue;
                if (seats >= waitingList[i].num) {
                    update_ticket(trainInfo, waitingList[i], train, false, s);
                    waitingList[i].status = success;
                    Waiting.erase(Yuki::pair<TrainTime, Ticket>(re_train, waitingList[i]));
                    Order.update(Yuki::pair<char, Ticket>(waitingList[i].user, waitingList[i]));
                }
            }
        }
        //train.trainData.update(Yuki::pair<char, TrainInfo> (trainInfo.trainID, trainInfo));
        refund.status = refunded;
        Order.update(Yuki::pair<char, Ticket> (u, refund));
        train.trainIndex.write(trainInfo, TrainSystem::indexToPos(bi.index));
        train.seatsIndex.write(s,  sizeof(Seats) * (durMax * trainInfo.index + dur));
    }

    static void clean() {
        std::filesystem::path path1("order.txt");
        std::filesystem::path path2("space_order.txt");
        std::filesystem::path path3("waiting.txt");
        std::filesystem::path path4("space_waiting.txt");
        if (std::filesystem::exists(path1)) std::filesystem::remove(path1);
        if (std::filesystem::exists(path2)) std::filesystem::remove(path2);
        if (std::filesystem::exists(path3)) std::filesystem::remove(path3);
        if (std::filesystem::exists(path4)) std::filesystem::remove(path4);
    }
};
#endif //TICKETSYSTEM_TICKETSYSTEM_HPP
