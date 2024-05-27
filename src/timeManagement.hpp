#ifndef TICKETSYSTEM_TIMEMANAGEMENT_HPP
#define TICKETSYSTEM_TIMEMANAGEMENT_HPP
#include<string>
#include<iomanip>
#include<iostream>
#include"../BPT/utility.hpp"
int Month[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
class Time{
    friend class Day;
    friend class TrainSystem;
    friend class StationInfo;
    friend class TrainInfo;
    friend class TokenScanner;
    friend class TimeManagement;
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
    friend class TimeManagement;
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
    friend class TimeManagement;
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

class TimeManagement {
public:
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
};
#endif //TICKETSYSTEM_TIMEMANAGEMENT_HPP
