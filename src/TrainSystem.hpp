#ifndef TICKETSYSTEM_TRAINSYSTEM_HPP
#define TICKETSYSTEM_TRAINSYSTEM_HPP
#include<cstring>
#include<string>
#include"../BPT/vector.hpp"
#include"../BPT/BPTree.hpp"
#include"../BPT/priority_queue.hpp"

int Month[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
class Time{
    friend class Day;
    friend class TrainSystem;
    friend class StationInfo;
    friend class TrainInfo;
    friend class TokenScanner;
private:
    int hour = -1;
    int minute = -1;
public:
    Time() {
        hour = -1;
        minute = -1;
    }
    explicit Time(int h, int m) :hour(h), minute(m){}
    std::string change(int num) const {
        std::string ans;
        if (num == -1) {
            ans = "xx";
            return ans;
        }
        std::string ans1(1, num / 10 + '0');
        std::string ans2(1, num % 10 + '0');
        ans = ans1 + ans2;
        return ans;
    }

    Time& operator=(const Time& other) {
        if (this == &other) return *this;
        hour = other.hour;
        minute = other.minute;
        return *this;
    }

    static bool checkTime(const Time& t) {
        if (t.minute >= 60) return false;
        if (t.hour >= 24) return false;
        return true;
    }

    bool operator< (const Time& other) const {
        if (hour < other.hour) return true;
        if (hour == other.hour) {
            if (minute < other.minute) return true;
            else return false;
        } else return false;
    }

    int operator-(const Time& other) const {
        int d_h = hour - other.hour;
        int d_m = minute - other.minute;
        return d_h * 60 + d_m;
    }

    bool operator==(const Time &other) const {
        return (hour == other.hour) && (minute == other.minute);
    }

    friend std::ostream& operator<< (std::ostream& os, const Time &obj) {
        os<<obj.change(obj.hour)<<':'<<obj.change(obj.minute);
        return os;
    }
};
class Day{
    friend class Time;
    friend class Date;
    friend class TrainSystem;
    friend class TokenScanner;
private:
    int month = -1;
    int day = -1;
public:
    Day() {
        month = -1;
        day = -1;
    }
    explicit Day(int m, int d) : month(m), day(d){}
    std::string change(int num, bool flag = true) const {
        std::string ans;
        if (num == -1) {
            ans = "xx";
            return ans;
        }
        std::string ans1(1, num / 10 + '0');
        std::string ans2(1, num % 10 + '0');
        ans = ans1 + ans2;
        return ans;
    }
    static bool checkDay(const Day& d) {
        if (d.month > 12 || d.month <= 0) return false;
        if (d.day > Month[d.month] || d.day < 0) return false;
        return true;
    }
    bool operator==(const Day &other) const {
        return day == other.day && month == other.month;
    }
    bool operator < (const Day &other) const {
        if (month < other.month) return true;
        else if (month > other.month) return false;
        else return day < other.day;
    }
    bool operator != (const Day &other) const {
        return !(*this == other);
    }
    Day &operator=(const Day &other) {
        if (this == &other) return *this;
        month = other.month;
        day = other.day;
        return *this;
    }

    int operator-(Day &other) const {
        if (month == other.month) {
            return day - other.day;
        }
        int sum = 0;
        if (month < other.month) {
            for (int i = month + 1; i < other.month; i++) sum += Month[i];
            sum += other.day;
            sum += (Month[month] - day);
            return sum;
        }
        if (month > other.month) {
            for (int i = other.month + 1; i < month; i++) sum += Month[i];
            sum += day;
            sum += (Month[other.month] - other.day);
            return sum;
        }
    }

    Day operator++(int n) {
        Day temp = *this;
        if (day == Month[month]) {
            day = 1;
            month++;
        }
        else day++;
        return temp;
    }
    friend std::ostream& operator<<(std::ostream &os, const Day &obj) {
        os<<obj.change(obj.month)<<'-'<<obj.change(obj.day);
        return os;
    }
};
class Date{
    friend class TicketSystem;
    friend class TrainInfo;
    friend class TrainSystem;
    friend class TokenScanner;
private:
    Day st;
    Day en;
public:
    Date() = default;
    explicit Date(Day a, Day b) :st(a), en(b){}
    bool check(const Day &day) const {
        if (day.month < st.month || day.month > en.month) return false;
        if (day.month == st.month) {
            if (day.day < st.day || day.day <= 0) return false;
        }
        if (day.month == en.month) {
            if (day.day > en.day || day.day > Month[en.month]) return false;
        }
        for (int i = st.month + 1; i < en.month; i++) {
            if (day.day <= 0 || day.day > Month[i]) return false;
        }
        return true;
    }
    Date& operator=(const Date &other) {
        if (this == &other) return *this;
        st = other.st;
        en = other.en;
        return *this;
    }
};
class StationInfo{
    friend class TrainSystem;
    friend class TrainInfo;
    friend class TicketSystem;
private:
    char name[30] = {'\0'};
    int price = 0; // 第一站的票价为0
    int arriveTime = 0;
    int leaveTime = 0;
    int remainSeats[100] = {0}; // 对seats利用差分数组
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
        for (int i = 0; i < 100; i++) {
            remainSeats[i] = other.remainSeats[i];
        }
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
    StationInfo stations[22]; // 实际的车站顺序
    Date date;
    bool isRelease = false;
public:
    TrainInfo(){}
    explicit TrainInfo(const char* id, int n, char t, Date date_, Time time, bool flag) :stationNum(n), type(t),date(date_), ini_time(time), isRelease(flag) {
        strcpy(trainID, id);
    }
    explicit TrainInfo(const TrainInfo& other) {
        strcpy(trainID, other.trainID);
        stationNum = other.stationNum;
        type = other.type;
        ini_time = other.ini_time;
        date = other.date;
        for (int i = 0; i <= stationNum; i++) {
            stations[i] = other.stations[i];
        }
        isRelease = other.isRelease;
    }

    TrainInfo& operator=(const TrainInfo& other) {
        if (this == &other) return *this;
        strcpy(trainID, other.trainID);
        stationNum = other.stationNum;
        type = other.type;
        ini_time = other.ini_time;
        date = other.date;
        for (int i = 0; i <= stationNum; i++) {
            stations[i] = other.stations[i];
        }
        isRelease = other.isRelease;
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
class compInfo{
    friend class timeComp;
    friend class costComp;
    friend class TrainSystem;
    friend class TransComp;
private:
    int price = 0;
    int time = 0;
    TrainInfo train;
    int st = 0;
    int en = 0;
public:
    compInfo() = default;
    explicit compInfo(int p, int t, TrainInfo& tr, int s, int e) :price(p), time(t), st(s), en(e) {
        train = tr;
    }
    compInfo& operator=(const compInfo& other) {
        if (this == &other) return *this;
        price = other.price;
        time = other.price;
        st = other.st;
        en = other.en;
        train = other.train;
        return *this;
    }
};
class timeComp{
public:
    bool operator() (const compInfo& a, const compInfo& b) const {
        if (a.time > b.time) return true;
        else if (a.time == b.time) {
            if (strcmp(a.train.trainID, b.train.trainID) > 0) return true;
            else return false;
        } else return false;
    }
};
class costComp{
public:
    bool operator() (const compInfo& a, const compInfo& b) const {
        if (a.price > b.price) return true;
        else if (a.price == b.price) {
            if (strcmp(a.train.trainID, b.train.trainID) > 0) return true;
            else return false;
        } else return false;
    }
};
class TrainFunction{
public:
    int operator() (const int &num) {
        return num;
    }
};
template <class T, class cmp = std::less<T>>
class Sort{
private:
    cmp comp;
    void merge(T arr[], int start, int mid, int end) {
        T result[end - start + 5];
        int k = 0;
        int i = start;
        int j = mid + 1;
        while (i <= mid && j <= end) {
            if (comp(arr[i], arr[j])){
                result[k++] = arr[i++];
            }
            else{
                result[k++] = arr[j++];
            }
        }
        if (i == mid + 1) {
            while(j <= end)
                result[k++] = arr[j++];
        }
        if (j == end + 1) {
            while (i <= mid)
                result[k++] = arr[i++];
        }
        for (j = 0, i = start ; j < k; i++, j++) {
            arr[i] = result[j];
        }
    }
public:
    void mergeSort(T arr[], int start, int end) {
        if (start >= end)
            return;
        int mid = ( start + end ) / 2;
        mergeSort(arr, start, mid);
        mergeSort(arr, mid + 1, end);
        merge(arr, start, mid, end);
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
public:
    TransComp() = default;
    explicit TransComp(int t, int c) : time(t), cost(c) {}
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
        return *this;
    }
    bool comp_cost(const TransComp& other) {
        if (cost > other.cost) return true;
        if (cost == other.cost) {
            if (time > other.time) return true;
            else if (time < other.time) return false;
            else {
                if (strcmp(first.train.trainID, other.first.train.trainID) > 0) return true;
                else if (strcmp(first.train.trainID, other.first.train.trainID) < 0) return false;
                else {
                    if (strcmp(second.train.trainID, other.second.train.trainID) > 0) return true;
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
                if (strcmp(first.train.trainID, other.first.train.trainID) > 0) return true;
                else if (strcmp(first.train.trainID, other.first.train.trainID) < 0) return false;
                else {
                    if (strcmp(second.train.trainID, other.second.train.trainID) > 0) return true;
                    else return false;
                }
            }
        } else return false;
    }
};

class TrainSystem{
    friend class TicketSystem;
    friend class TokenScanner;
    friend class TokenScanner;
private:
    // trainId在trainIndex里对应index的位置
    BPT<char, int, TrainFunction, 20, 2, 1024> trainData;
    // <station, index> station里面的所有trainId的index
    BPT<char, int, TrainFunction, 20, 2, 1024> stationData; // 管理的是release 以后的车次
    int total_index = 0;
    Yuki::vector<int> allIndex;
    FileSystem<TrainInfo, 2> trainIndex; // 第一个是total_index
    FileSystem<int, 2> deleteIndex; // 删除的train空间回收
    Yuki::HashMap<int, TrainInfo, TrainFunction, 53, 200> Buffer;
    // index 0-based
    static ll indexToPos(int index) {
        return 2 * sizeof(int) + index * sizeof(TrainInfo);
    }

    // 发车时间距离st多少天，now是现在的时间，不是这一班车的发车时间
    static int cal_now(const Date &d, const Day &now) {
        int sum = 0;
        for (int i = d.st.month + 1; i < now.month; i++) {
            sum += Month[i];
        }
        if (d.st.month != now.month) {
            sum += (Month[d.st.month] - d.st.day + 1);
            sum += now.day;
        } else {
            sum += (now.day - d.st.day + 1);
        }
        return sum;
    }
    static Yuki::pair<Day, Time> showTime(const Day &d, const Time &t, int num) {
        // show到达本站和离开本站的绝对时间
        // d   t每天的始发时间 num到本站所用的时间
        Day day = d;
        Time time = t;
        time.minute += num;
        if (time.minute >= 60) {
            time.hour += (time.minute / 60);
            time.minute = time.minute % 60;
        }
        if (time.hour >= 24) {
            day.day += (time.hour / 24);
            time.hour = time.hour % 24;
        }
        if (day.day > Month[day.month]) {
            day.day -= Month[day.month];
            day.month += 1;
        }
        return {day, time};
    }
    static Day checkBegin(const Day &now, const Time &ini, int num) {
        Time time = ini;
        Day st = now;
        time.minute += num;
        if (time.minute >= 60) {
            time.hour += (time.minute / 60);
            time.minute = time.minute % 60;
        }
        int d_plus = 0, month_plus = 0;
        if (time.hour >= 24) {
            d_plus = time.hour / 24;
            time.hour = time.hour % 24;
        }
        if (now.day <= d_plus) {
            st.month--;
            st.day = Month[st.month] + now.day - d_plus;
        } else {
            st.day -= d_plus;
        }
        return st;
    }
    void trans_print(const compInfo &info, const Yuki::pair<Day, Time> &leave, const Yuki::pair<Day, Time> &arrive) {
        query_print(info.train, leave.first, info.train.stations[info.st].name, info.train.stations[info.en].name, leave, arrive, info.price, info.st, info.en);
    }
    // 在d这天买的train_info车次从s_index到t_index
    static int cal_ticket(const TrainInfo &train_info, const Day& d, const int &s_index, const int &t_index) {
        int cnt = 0;
        int min = 100005;
        Day st_day = checkBegin(d, train_info.ini_time, train_info.stations[s_index].leaveTime);
        int dur_now = cal_now(train_info.date, st_day);
        for (int k = s_index; k < t_index; k++) {
            cnt += train_info.stations[k].remainSeats[dur_now];
            if (cnt < min) min = cnt;
        }
        int pre_ticket = train_info.stations[0].remainSeats[0];
        for (int k = 0; k < s_index; k++) {
            pre_ticket += train_info.stations[k].remainSeats[dur_now];
        }
        int min_ticket = pre_ticket + min;
        return min_ticket;
    }
    static int cal_ticket_(const TrainInfo &train_info, const Day& d, const int &s_index, const int &t_index) {
        int cnt = 0;
        int min = 100005;
        int dur_now = cal_now(train_info.date, d);
        for (int k = s_index; k < t_index; k++) {
            cnt += train_info.stations[k].remainSeats[dur_now];
            if (cnt < min) min = cnt;
        }
        int pre_ticket = train_info.stations[0].remainSeats[0];
        for (int k = 0; k < s_index; k++) {
            pre_ticket += train_info.stations[k].remainSeats[dur_now];
        }
        int min_ticket = pre_ticket + min;
        return min_ticket;
    }
    static void query_print(const TrainInfo& train_info, const Day &d, const char* s, const char *t, Yuki::pair<Day, Time> leave, const Yuki::pair<Day, Time> &arrive, const int &p, const int &s_index, const int &t_index) {
        std::cout<<train_info.trainID<<' '<<s<<' ';
        std::cout<<leave.first<<' '<<leave.second<<" -> ";
        std::cout<<t<<' '<<arrive.first<<' '<<arrive.second<<' '<<p<<' ';
        std::cout<<cal_ticket(train_info, d, s_index, t_index)<<'\n';
    }
    /*int findDestination(const char obj[],const TrainInfo &key, int l, int r) {
        if (l > r) return -1;
        int mid = (l + r) / 2;
        if (strcmp(obj, key.sortStation[mid].stationName) < 0) return findDestination(obj, key, l, mid - 1);
        else if (strcmp(obj, key.sortStation[mid].stationName) == 0) return key.sortStation[mid].index;
        else return findDestination(obj, key, mid + 1, r);
    }*/
public:
    // 考虑stationData是否可以变为<char, char>
    TrainSystem(): trainData("train.txt", "space_train.txt"), stationData("station.txt", "space_station.txt"){
        trainIndex.initialise("trainIndex.txt");
        trainIndex.get_info(total_index, 1);
        deleteIndex.initialise("delete.txt");
        int total = 0;
        deleteIndex.get_info(total, 1);
        for (int i = 0; i < total; i++) {
            int num;
            deleteIndex.read(num, 2 * sizeof(int) + i * sizeof(int));
            allIndex.push_back(num);
        }
    }
    ~TrainSystem() {
        trainIndex.write_info(total_index, 1);
        deleteIndex.write_info((int)allIndex.size(), 1);
        for (int i = 0; i < allIndex.size(); i++) {
            deleteIndex.write(allIndex[i], 2 * sizeof(int) + i * sizeof(int));
        }
        Buffer.clearing(trainIndex, 2);
    }

    int add_train(const char* i, int n, int m, const Yuki::vector<string> &s, const Yuki::vector<int> &p, Time x, const Yuki::vector<int> &t, const Yuki::vector<int> &o, const Date &d, char y) {
        TrainInfo train_info(i, n, y, d, x, false);
        int info_index;
        bool exist = trainData.findKV(i, info_index);
        if (exist) return -1;
        train_info.ini_time = x;
        train_info.stations[0].remainSeats[0] = m;
        for (int j = 1; j <= n; j++) {
            strcpy(train_info.stations[j].name, s[j - 1].c_str());
            if (j == 1) {
                train_info.stations[j].leaveTime = 0;
                train_info.stations[j].price = 0;
            }
            if (j != 1 && j != n) {
                train_info.stations[j].price = train_info.stations[j - 1].price + p[j - 2];
                train_info.stations[j].arriveTime = (t[j - 2] + train_info.stations[j - 1].leaveTime);
                train_info.stations[j].leaveTime = (train_info.stations[j].arriveTime + o[j - 2]);
            }
            if (j == n) {
                train_info.stations[n].price = train_info.stations[n - 1].price + p[n - 2];
                train_info.stations[n].arriveTime = (train_info.stations[n - 1].leaveTime + t[n - 2]);
            }
        }
        //Sort<SortStation, std::less<SortStation>> sort;
        //sort.mergeSort(train_info.sortStation, 1, n);
        if (allIndex.empty()) info_index = total_index++;
        else {
            info_index = allIndex.back();
            allIndex.pop_back();
        }
        trainData.insert(Yuki::pair<char, int>(i, info_index));
        trainIndex.write(train_info, indexToPos(info_index));
        Buffer.insert(info_index, train_info, trainIndex, 2, false);
        return 0;
    }
    int delete_train(const char *i) {
        TrainInfo d_info;
        int info_index;
        bool exist = trainData.findKV(i, info_index);
        if (!exist) return -1;
        if (!Buffer.find(info_index, d_info))
            trainIndex.read(d_info, indexToPos(info_index));
        if (d_info.isRelease) return -1;
        else {
            for (int j = 1; j <= d_info.stationNum; j++) {
                stationData.erase(Yuki::pair<char, int> (d_info.stations[j].name, info_index));
            }
            trainData.erase(Yuki::pair<char, int> (i, info_index));
            Buffer.erase(info_index);
            allIndex.push_back(info_index);
        }
        return 0;
    }
    int release_train(const char* i) {
        TrainInfo r_info;
        int info_index;
        bool exist = trainData.findKV(i, info_index);
        if (!exist) return -1;
        if (!Buffer.find(info_index, r_info))
            trainIndex.read(r_info, indexToPos(info_index));
        if (r_info.isRelease) return -1;
        r_info.isRelease = true;
        trainIndex.write(r_info, indexToPos(info_index));
        for (int j = 1; j <= r_info.stationNum; j++) {
            stationData.insert(Yuki::pair<char, int> (r_info.stations[j].name, info_index));
        }
        Buffer.insert(info_index, r_info, trainIndex, 2, false);
        return 0;
    }
    Yuki::pair<TrainInfo, bool> query_train(const char *i, Day& day) {
        TrainInfo q_info;
        int info_index;
        bool exist = trainData.findKV(i, info_index);
        if (!exist) return {q_info, false};
        if (!Buffer.find(info_index, q_info))
            trainIndex.read(q_info, indexToPos(info_index));
        bool isDue = q_info.date.check(day);
        Buffer.insert(info_index, q_info, trainIndex, 2, false);
        if (!isDue) return {q_info, false};
        return {q_info, true};
    }
    // 在query_train时print
    // 在Day d时发车
    static void printTrain(Day d, TrainInfo &train_info) {
        std::cout<<train_info.trainID<<' '<<train_info.type<<'\n';
        int cur_seat = train_info.stations[0].remainSeats[0];
        for (int i = 1; i <= train_info.stationNum; i++) {
            std::cout<<train_info.stations[i].name<<' ';
            Yuki::pair<Day, Time> arrive = showTime(d, train_info.ini_time, train_info.stations[i].arriveTime);
            Yuki::pair<Day, Time> leave = showTime(d, train_info.ini_time, train_info.stations[i].leaveTime);
            if (i == 1) std::cout<<"xx-xx xx:xx";
            else std::cout << arrive.first << ' ' << arrive.second;
            if (i == train_info.stationNum) std::cout<<" -> xx-xx xx:xx ";
            else std::cout << " -> " << leave.first << ' ' << leave.second << ' ';
            std::cout<<train_info.stations[i].price<<' ';
            // 这是发车的第几天
            int dur_now = cal_now(train_info.date, d);
            if (i == train_info.stationNum) std::cout<<"x\n";
            else {
                cur_seat += train_info.stations[i].remainSeats[dur_now];
                std::cout<<cur_seat<<'\n';
            }
        }
    }
    // flag->true  time    false cost
    void query_ticket(const char* s, const char *t, const Day& d, bool flag) {
        Yuki::vector<int> all_st, all_en, all;
        all_st = stationData.find(s); // 均已经release过
        all_en = stationData.find(t);
        Yuki::priority_queue<compInfo, timeComp> tq;
        Yuki::priority_queue<compInfo, costComp> cq;
        if (all_st.empty() || all_en.empty()) {
            std::cout<<"0\n";
            return;
        }
        int m = 0, n = 0;
        while (m < all_st.size() && n < all_en.size()) {
            if (all_st[m] < all_en[n]) {
                m++;
                continue;
            }
            if (all_st[m] > all_en[n]) {
                n++;
                continue;
            }
            if (all_st[m] == all_en[n]) {
                all.push_back(all_st[m]);
                m++;
                n++;
            }
        }
        for (int i = 0; i < all.size(); i++) {
            // 二分查找看目的地是否存在，利用sortStation
            TrainInfo objTrain;
            if (!Buffer.find(all[i], objTrain))
                trainIndex.read(objTrain, indexToPos(all[i]));
            int res = 1;
            for (; res <= objTrain.stationNum; res++) {
                if (strcmp(s, objTrain.stations[res].name) == 0) break;
            }
            StationInfo st = objTrain.stations[res];
            Day earliest(showTime(objTrain.date.st, objTrain.ini_time, st.leaveTime).first);
            Day latest(showTime(objTrain.date.en, objTrain.ini_time, st.leaveTime).first);
            Date dur(earliest, latest);
            if (!dur.check(d)) continue; // 不在发车时间段内
            int ans = 1;
            for (; ans <= objTrain.stationNum; ans++) {
                if (strcmp(t, objTrain.stations[ans].name) == 0) break;
            }
            if (ans == objTrain.stationNum + 1 || res == objTrain.stationNum + 1) continue;
            if (res > ans) continue;
            if (ans != -1) {
                // 读取真正的trainInfo
                int cost = objTrain.stations[ans].price - objTrain.stations[res].price;
                int time_ = objTrain.stations[ans].arriveTime - objTrain.stations[res].leaveTime;
                if (flag) tq.push(compInfo(cost, time_, objTrain, res, ans));
                else cq.push(compInfo(cost, time_, objTrain, res, ans));
            }
        }
        if (flag) {
            std::cout<<tq.size()<<'\n';
            while (!tq.empty()) {
                compInfo top = tq.top();
                tq.pop();
                Day st = checkBegin(d, top.train.ini_time, top.train.stations[top.st].leaveTime);
                Yuki::pair<Day, Time> leave = showTime(st, top.train.ini_time, top.train.stations[top.st].leaveTime);
                Yuki::pair<Day, Time> arrive = showTime(st, top.train.ini_time, top.train.stations[top.en].arriveTime);
                query_print(top.train, d, s, t, leave, arrive, top.price, top.st, top.en);
            }
        } else {
            std::cout<<cq.size()<<'\n';
            while (!cq.empty()) {
                compInfo top = cq.top();
                cq.pop();
                Day st = checkBegin(d, top.train.ini_time, top.train.stations[top.st].leaveTime);
                Yuki::pair<Day, Time> leave = showTime(st, top.train.ini_time, top.train.stations[top.st].leaveTime);
                Yuki::pair<Day, Time> arrive = showTime(st, top.train.ini_time, top.train.stations[top.en].arriveTime);
                query_print(top.train, d, s, t, leave, arrive, top.price, top.st, top.en);
            }
        }
    }
    void query_transfer(const char* s, const char *t, const Day& d, bool flag){
        Yuki::vector<int> all;
        all = stationData.find(s);
        TransComp transComp;
        bool f = false;
        for (int i = 0; i < all.size(); i++) {
            TrainInfo firTrain;
            if (!Buffer.find(all[i], firTrain))
                trainIndex.read(firTrain, indexToPos(all[i]));
            int res = 1;
            for (; res <= firTrain.stationNum; res++) {
                if (strcmp(s, firTrain.stations[res].name) == 0) break;
            }
            StationInfo st = firTrain.stations[res];// 始发站信息
            Day start_time = checkBegin(d, firTrain.ini_time, st.leaveTime);
            if (!firTrain.date.check(start_time)) continue;
            Yuki::pair<Day, Time> leave_st = showTime(start_time, firTrain.ini_time, st.leaveTime);
            for (int j = res + 1; j <= firTrain.stationNum; j++) {
                StationInfo mid = firTrain.stations[j];
                // 到达中转站的时间
                Yuki::pair<Day, Time> arrive_mid = showTime(start_time, firTrain.ini_time, mid.arriveTime);
                // 中转站里的所有车辆信息
                Yuki::vector<int> mid_all = stationData.find(mid.name);
                for (int k = 0; k < mid_all.size(); k++) {
                    TrainInfo secTrain;
                    if (!Buffer.find(mid_all[k], secTrain))
                        trainIndex.read(secTrain, indexToPos(mid_all[k]));
                    if (secTrain == firTrain) continue;// 不能是同一辆车
                    int m_index = 1;
                    for (; m_index <= secTrain.stationNum; m_index++) {
                        if (strcmp(mid.name, secTrain.stations[m_index].name) == 0) break;
                    }
                    int end = m_index + 1;
                    for (; end <= secTrain.stationNum; end++) {
                        if (strcmp(t, secTrain.stations[end].name) == 0) break;
                    }
                    if (end == secTrain.stationNum + 1) continue;
                    if (m_index >= end) continue;
                    // 在到达后继续换乘
                    StationInfo en = secTrain.stations[end];
                    // 换乘车辆的发车时间
                    Day mid_earliest(showTime(secTrain.date.st, secTrain.ini_time, secTrain.stations[m_index].leaveTime).first);
                    Day mid_latest(showTime(secTrain.date.en, secTrain.ini_time, secTrain.stations[m_index].leaveTime).first);
                    Date mid_dur(mid_earliest, mid_latest);
                    Day mid_start;
                    Yuki::pair<Day, Time> mid_arrive;
                    if (mid_dur.check(arrive_mid.first)) {
                        // 到达时间在范围内，mid_arrive计算离开时间 ，不用考虑离开的时候是否在同一天
                        mid_start = checkBegin(arrive_mid.first, secTrain.ini_time, secTrain.stations[m_index].leaveTime);
                        mid_arrive = showTime(mid_start, secTrain.ini_time, secTrain.stations[m_index].leaveTime);
                        if (mid_arrive.second < arrive_mid.second && mid_arrive.first == arrive_mid.first) {
                            mid_arrive.first++;
                            mid_start++;
                            if (!mid_dur.check(mid_arrive.first)) continue;
                        }
                    } else {
                        if (mid_latest < arrive_mid.first) continue;
                        mid_start = secTrain.date.st;
                        mid_arrive = showTime(mid_start, secTrain.ini_time, secTrain.stations[m_index].leaveTime);
                    }
                    Yuki::pair<Day, Time> en_arrive = showTime(mid_start, secTrain.ini_time, en.arriveTime);
                    int all_time = 0;
                    int all_cost = 0;
                    all_time = mid.arriveTime - st.leaveTime + en.arriveTime - secTrain.stations[m_index].leaveTime;
                    all_time += ((mid_arrive.second - arrive_mid.second) + 24 * 60 * (mid_arrive.first - arrive_mid.first));
                    all_cost = (mid.price - st.price + en.price - secTrain.stations[m_index].price);
                    compInfo fir(mid.price - st.price, mid.arriveTime - st.arriveTime, firTrain, res, j);
                    compInfo sec(en.price - secTrain.stations[m_index].price, en.arriveTime -secTrain.stations[m_index].leaveTime,
                                 secTrain, m_index, end);
                    if (!f) {
                        transComp.cost = all_cost;
                        transComp.time = all_time;
                        transComp.first = fir;
                        transComp.second = sec;
                        transComp.st_lea = leave_st;
                        transComp.mid_arr = arrive_mid;
                        transComp.mid_lea = mid_arrive;
                        transComp.en_arr = en_arrive;
                        f = true;
                    } else {
                        TransComp other(all_time, all_cost);
                        other.first = fir;
                        other.mid_arr = arrive_mid;
                        other.mid_lea = mid_arrive;
                        other.en_arr = en_arrive;
                        other.st_lea = leave_st;
                        other.second = sec;
                        if (flag) {
                            if (transComp.comp_time(other)) transComp = other;
                        } else {
                            if (transComp.comp_cost(other)) transComp = other;
                        }
                    }
                }
            }
        }
        if (!f) std::cout<<"0\n";
        else {
            trans_print(transComp.first, transComp.st_lea, transComp.mid_arr);
            trans_print(transComp.second, transComp.mid_lea, transComp.en_arr);
        }
    }
    static void clean() {
        std::filesystem::path path1("train.txt");
        std::filesystem::path path2("space_train.txt");
        std::filesystem::path path3("station.txt");
        std::filesystem::path path4("space_station.txt");
        if (std::filesystem::exists(path1)) std::filesystem::remove(path1);
        if (std::filesystem::exists(path2)) std::filesystem::remove(path2);
        if (std::filesystem::exists(path3)) std::filesystem::remove(path3);
        if (std::filesystem::exists(path4)) std::filesystem::remove(path4);
    }

    bool checking() {
        char id[24] = "LeavesofGrass";
        int info_index;
        trainData.findKV(id, info_index);
        TrainInfo info;
        trainIndex.read(info, indexToPos(info_index));
        Day day(6, 16);
        if (cal_ticket(info, day, 1, 6) < 0) return false;
        else return true;
    }
};


#endif //TICKETSYSTEM_TRAINSYSTEM_HPP