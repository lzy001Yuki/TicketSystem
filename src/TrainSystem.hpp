#ifndef TICKETSYSTEM_TRAINSYSTEM_HPP
#define TICKETSYSTEM_TRAINSYSTEM_HPP
#include "trainType.hpp"
class TrainSystem{
    friend class TicketSystem;
    friend class TokenScanner;
    friend class TokenScanner;
private:
    // trainId在trainIndex里对应index的位置
    BPT<char, basicInfo, TrainFunction, 30, 2, 1024> trainData;
    // <station, index> station里面的所有trainId的index
    BPT<char, TrainSta, TrainFunction, 30, 2, 1024> stationData; // 管理的是release 以后的车次
    int total_index = 0;
    Yuki::vector<int> allIndex;
    FileSystem<TrainInfo, 2> trainIndex; // 第一个是total_index，储存完整的trainInfo
    FileSystem<int, 2> deleteIndex; // 删除的train空间回收
    FileSystem<Seats, 0> seatsIndex;

    // index 0-based
    static ll indexToPos(int index,  int info_len = 2) {
        return info_len * sizeof(int) + index * sizeof(TrainInfo);
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
    void trans_print(const compInfo &c_info, const Yuki::pair<Day, Time> &leave, const Yuki::pair<Day, Time> &arrive, const char *s, const char *e) {
        query_print(c_info, leave.first, s, e, leave, arrive, c_info.st, c_info.en);
    }
    // 在d这天买的train_info车次从s_index到t_index
    int cal_ticket(const int &s_index, const int &t_index, Seats& seats, const int &max) {
        int cnt = 0;
        int min = 100005;
        for (int k = s_index; k < t_index; k++) {
            cnt += seats.seat[k];
            if (cnt < min) min = cnt;
        }
        int pre_ticket = max;
        for (int k = 1; k < s_index; k++) {
            pre_ticket += seats.seat[k];
        }
        int min_ticket = pre_ticket + min;
        return min_ticket;
    }
    void query_print(const compInfo& train_info, const Day &d, const char* s, const char *t, Yuki::pair<Day, Time> leave, const Yuki::pair<Day, Time> &arrive, int s_index, int t_index) {
        std::cout<<train_info.trainID<<' '<<s<<' ';
        std::cout<<leave.first<<' '<<leave.second<<" -> ";
        std::cout<<t<<' '<<arrive.first<<' '<<arrive.second<<' '<<train_info.price<<' ';
        Seats seats;
        Day st_day = checkBegin(d, train_info.info.ini, train_info.st_lt);
        int dur_now = cal_now(train_info.info.date, st_day);// 1-based 是发车的第几天
        seatsIndex.read(seats, sizeof(Seats) * (durMax * train_info.info.index + dur_now));
        std::cout<<cal_ticket(s_index, t_index, seats, train_info.info.maxSeats)<<'\n';
    }
public:
    // 考虑stationData是否可以变为<char, char>
    TrainSystem(): trainData("train.txt", "space_train.txt"), stationData("station.txt", "space_station.txt"){
        trainIndex.initialise("trainIndex.txt");
        seatsIndex.initialise("seats.txt");
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
    }

    int add_train(const char* i, int n, int m, const Yuki::vector<string> &s, const Yuki::vector<int> &p, const Time& x, const Yuki::vector<int> &t, const Yuki::vector<int> &o, const Date &d, char y) {
        TrainInfo train_info(i, n, y, d, x, false, m);
        int info_index;
        if (allIndex.empty()) info_index = total_index++;
        else {
            info_index = allIndex.back();
            allIndex.pop_back();
        }
        train_info.index = info_index;
        basicInfo bi(d, x, info_index, m);
        bool exist = trainData.findKV(i, bi);
        if (exist) return -1;
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
        trainData.insert(Yuki::pair<char, basicInfo>(i, bi));
        trainIndex.write(train_info, indexToPos(info_index));
        return 0;
    }
    int delete_train(const char *i) {
        TrainInfo d_info;
        basicInfo bi;
        bool exist = trainData.findKV(i, bi);
        if (!exist) return -1;
        trainIndex.read(d_info, indexToPos(bi.index));
        if (d_info.isRelease) return -1;
        else {
            trainData.erase(Yuki::pair<char, basicInfo> (i, bi));
            allIndex.push_back(bi.index);
        }
        return 0;
    }
    int release_train(const char* i) {
        TrainInfo r_info;
        basicInfo bi;
        bool exist = trainData.findKV(i, bi);
        if (!exist) return -1;
        trainIndex.read(r_info, indexToPos(bi.index));
        if (r_info.isRelease) return -1;
        r_info.isRelease = true;
        trainIndex.write(r_info, indexToPos(bi.index));
        for (int j = 1; j <= r_info.stationNum; j++) {
            TrainSta ts(i, bi, j, r_info.stations[j].price, r_info.stations[j].arriveTime, r_info.stations[j].leaveTime);
            stationData.insert(Yuki::pair<char, TrainSta> (r_info.stations[j].name, ts));
        }
        return 0;
    }
    Yuki::pair<TrainInfo, bool> query_train(const char *i, Day& day) {
        TrainInfo q_info;
        basicInfo bi;
        bool exist = trainData.findKV(i, bi);
        if (!exist) return {q_info, false};
        trainIndex.read(q_info, indexToPos(bi.index));
        bool isDue = q_info.date.check(day);
        if (!isDue) return {q_info, false};
        return {q_info, true};
    }
    // 在query_train时print
    // 在Day d时发车
    void printTrain(Day d, TrainInfo &train_info) {
        std::cout<<train_info.trainID<<' '<<train_info.type<<'\n';
        int cur_seat = train_info.maxSeats;
        // 这是发车的第几天
        int dur_now = cal_now(train_info.date, d);
        Seats seats;
        seatsIndex.read(seats, sizeof(Seats) * (durMax * train_info.index + dur_now));
        for (int i = 1; i <= train_info.stationNum; i++) {
            std::cout<<train_info.stations[i].name<<' ';
            Yuki::pair<Day, Time> arrive = showTime(d, train_info.ini_time, train_info.stations[i].arriveTime);
            Yuki::pair<Day, Time> leave = showTime(d, train_info.ini_time, train_info.stations[i].leaveTime);
            if (i == 1) std::cout<<"xx-xx xx:xx";
            else std::cout << arrive.first << ' ' << arrive.second;
            if (i == train_info.stationNum) std::cout<<" -> xx-xx xx:xx ";
            else std::cout << " -> " << leave.first << ' ' << leave.second << ' ';
            std::cout<<train_info.stations[i].price<<' ';
            if (i == train_info.stationNum) std::cout<<"x\n";
            else {
                cur_seat += seats.seat[i];
                std::cout<<cur_seat<<'\n';
            }
        }
    }
    // flag->true  time    false cost
    void query_ticket(const char* s, const char *t, const Day& d, bool flag) {
        Yuki::vector<TrainSta> all_st, all_en, all;
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
            if (all_st[m].info.index < all_en[n].info.index) {
                m++;
                continue;
            }
            if (all_st[m].info.index > all_en[n].info.index) {
                n++;
                continue;
            }
            if (all_st[m].info.index == all_en[n].info.index) {
                if (all_st[m].st_index >= all_en[n].st_index) {
                    m++;
                    n++;
                    continue;
                }
                all.push_back(all_st[m]);
                all.push_back(all_en[n]);
                m++;
                n++;
            }
        }
        for (int i = 0; i < all.size(); i += 2) {
            //TrainInfo objTrain;
            //trainIndex.read(objTrain, indexToPos(all[i].first));
            int res = all[i].st_index;
            //StationInfo st = objTrain.stations[res];
            Day earliest(showTime(all[i].info.date.st, all[i].info.ini, all[i].l_time).first);
            Day latest(showTime(all[i].info.date.en, all[i].info.ini, all[i].l_time).first);
            Date dur(earliest, latest);
            if (!dur.check(d)) continue; // 不在发车时间段内
            int ans = all[i + 1].st_index;
            if (ans != -1) {
                int cost = all[i + 1].price - all[i].price;
                int time_ = all[i + 1].a_time - all[i].l_time;
                if (flag) tq.push(compInfo(all[i].trainID, cost, time_, all[i].info, all[i].l_time, all[i + 1].a_time, res, ans));
                else cq.push(compInfo(all[i].trainID, cost, time_, all[i].info, all[i].l_time, all[i + 1].a_time, res, ans));
            }
        }
        if (flag) {
            std::cout<<tq.size()<<'\n';
            while (!tq.empty()) {
                compInfo top = tq.top();
                tq.pop();
                Day st = checkBegin(d, top.info.ini, top.st_lt);
                Yuki::pair<Day, Time> leave = showTime(st, top.info.ini, top.st_lt);
                Yuki::pair<Day, Time> arrive = showTime(st, top.info.ini, top.en_at);
                query_print(top, d, s, t, leave, arrive, top.st, top.en);
            }
        } else {
            std::cout<<cq.size()<<'\n';
            while (!cq.empty()) {
                compInfo top = cq.top();
                cq.pop();
                Day st = checkBegin(d, top.info.ini, top.st_lt);
                Yuki::pair<Day, Time> leave = showTime(st, top.info.ini, top.st_lt);
                Yuki::pair<Day, Time> arrive = showTime(st, top.info.ini, top.en_at);
                query_print(top, d, s, t, leave, arrive, top.st, top.en);
            }
        }
    }
    void query_transfer(const char* s, const char *t, const Day& d, bool flag){
        Yuki::vector<TrainSta> all_s, all_t;
        all_s = stationData.find(s);
        all_t = stationData.find(t);
        TransComp transComp;
        bool f = false;
        for (int i = 0; i < all_s.size(); i++) {
            TrainInfo firTrain;
            trainIndex.read(firTrain, indexToPos(all_s[i].info.index));
            int res = all_s[i].st_index;
            StationInfo st = firTrain.stations[res];// 始发站信息
            Day start_time = checkBegin(d, firTrain.ini_time, st.leaveTime);
            if (!firTrain.date.check(start_time)) continue;
            Yuki::pair<Day, Time> leave_st = showTime(start_time, firTrain.ini_time, st.leaveTime);
            /*for (int j = res + 1; j <= firTrain.stationNum; j++) {
                StationInfo mid = firTrain.stations[j];
                // 到达中转站的时间
                Yuki::pair<Day, Time> arrive_mid = showTime(start_time, firTrain.ini_time, mid.arriveTime);
                // 中转站里的所有车辆信息
                Yuki::vector<TrainSta> mid_all = stationData.find(mid.name);
                for (int k = 0; k < mid_all.size(); k++) {
                    TrainInfo secTrain;
                    trainIndex.read(secTrain, indexToPos(mid_all[k].info.index));
                    if (secTrain == firTrain) continue;// 不能是同一辆车
                    int m_index = mid_all[k].st_index;
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
                    compInfo fir(firTrain.trainID, mid.price - st.price, mid.arriveTime - st.arriveTime, all[i].info,
                                 st.leaveTime, mid.arriveTime, res, j);
                    compInfo sec(secTrain.trainID, en.price - secTrain.stations[m_index].price, en.arriveTime -secTrain.stations[m_index].leaveTime,
                                 mid_all[k].info, secTrain.stations[m_index].leaveTime, en.arriveTime, m_index, end);
                    if (!f) {
                        transComp.cost = all_cost;
                        transComp.time = all_time;
                        transComp.first = fir;
                        transComp.second = sec;
                        transComp.st_lea = leave_st;
                        transComp.mid_arr = arrive_mid;
                        transComp.mid_lea = mid_arrive;
                        transComp.en_arr = en_arrive;
                        strcpy(transComp.mid, mid.name);
                        f = true;
                    } else {
                        TransComp other(all_time, all_cost, mid.name);
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
            }*/
            for (int j = 0; j < all_t.size(); j++) {
                TrainInfo secTrain;
                if (all_s[i].info.index == all_t[j].info.index) continue;
                trainIndex.read(secTrain, indexToPos(all_t[j].info.index));
                int des = all_t[j].st_index;
                StationInfo end = secTrain.stations[des];
                for (int k = 1; k < des; k++) {
                    for (int z = res + 1; z <= firTrain.stationNum; z++) {
                        if (strcmp(firTrain.stations[z].name, secTrain.stations[k].name) != 0) continue;
                        int f_mid = z, s_mid = k;
                        StationInfo f_mid_st = firTrain.stations[f_mid], s_mid_st = secTrain.stations[s_mid];
                        Yuki::pair<Day, Time> arrive_mid = showTime(start_time, firTrain.ini_time, f_mid_st.arriveTime);
                        Day mid_earliest(showTime(secTrain.date.st, secTrain.ini_time, s_mid_st.leaveTime).first);
                        Day mid_latest(showTime(secTrain.date.en, secTrain.ini_time, s_mid_st.leaveTime).first);
                        Date mid_dur(mid_earliest, mid_latest);
                        Day mid_start;
                        Yuki::pair<Day, Time> mid_arrive;
                        if (mid_dur.check(arrive_mid.first)) {
                            // 到达时间在范围内，mid_arrive计算离开时间 ，不用考虑离开的时候是否在同一天
                            mid_start = checkBegin(arrive_mid.first, secTrain.ini_time, s_mid_st.leaveTime);
                            mid_arrive = showTime(mid_start, secTrain.ini_time, s_mid_st.leaveTime);
                            if (mid_arrive.second < arrive_mid.second && mid_arrive.first == arrive_mid.first) {
                                mid_arrive.first++;
                                mid_start++;
                                if (!mid_dur.check(mid_arrive.first)) continue;
                            }
                        } else {
                            if (mid_latest < arrive_mid.first) continue;
                            mid_start = secTrain.date.st;
                            mid_arrive = showTime(mid_start, secTrain.ini_time, s_mid_st.leaveTime);
                        }
                        Yuki::pair<Day, Time> en_arrive = showTime(mid_start, secTrain.ini_time, end.arriveTime);
                        int all_time = 0;
                        int all_cost = 0;
                        all_time = f_mid_st.arriveTime - st.leaveTime + end.arriveTime - s_mid_st.leaveTime;
                        all_time += ((mid_arrive.second - arrive_mid.second) + 24 * 60 * (mid_arrive.first - arrive_mid.first));
                        all_cost = (f_mid_st.price - st.price + end.price - s_mid_st.price);
                        compInfo fir(firTrain.trainID, f_mid_st.price - st.price, f_mid_st.arriveTime - st.arriveTime, all_s[i].info,
                                     st.leaveTime, f_mid_st.arriveTime, res, f_mid);
                        compInfo sec(secTrain.trainID, end.price - s_mid_st.price, end.arriveTime - s_mid_st.leaveTime,
                                     all_t[j].info, s_mid_st.leaveTime, end.arriveTime, s_mid, des);
                        if (!f) {
                            transComp.cost = all_cost;
                            transComp.time = all_time;
                            transComp.first = fir;
                            transComp.second = sec;
                            transComp.st_lea = leave_st;
                            transComp.mid_arr = arrive_mid;
                            transComp.mid_lea = mid_arrive;
                            transComp.en_arr = en_arrive;
                            strcpy(transComp.mid, s_mid_st.name);
                            f = true;
                        } else {
                            TransComp other(all_time, all_cost, s_mid_st.name);
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
        }
        if (!f) std::cout<<"0\n";
        else {
            trans_print(transComp.first, transComp.st_lea, transComp.mid_arr, s, transComp.mid);
            trans_print(transComp.second, transComp.mid_lea, transComp.en_arr, transComp.mid, t);
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
};


#endif //TICKETSYSTEM_TRAINSYSTEM_HPP