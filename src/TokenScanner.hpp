#ifndef TICKETSYSTEM_TOKENSCANNER_HPP
#define TICKETSYSTEM_TOKENSCANNER_HPP
#include<cstring>
#include "../BPT/vector.hpp"
#include "error.hpp"

class TokenScanner {
public:
    static char stringToType(const std::string &tmp) {
        if (tmp.size() > 1 || tmp.empty()) throw InvalidExp();
        if (tmp[0] < 'A' || tmp[0] > 'Z') throw InvalidExp();
        char ch[2] = {'\0'};
        strcpy(ch, tmp.c_str());
        return ch[0];
    }

    static void SplitString(const std::string &line, Yuki::vector<std::string> &parse) {
        std::string tmp;
        for (int i = 0; i <= line.length(); i++) {
            if (line[i] == ' ' || i == line.length()) {
                if (!tmp.empty()) parse.push_back(tmp);
                tmp = "";
                continue;
            }
            tmp += line[i];
        }
    }

    static int StringToInteger(const std::string &tmp) {
        int sum = 0;
        for (int i = 0; i < tmp.size(); i++) {
            if (i == tmp.size() - 1 && tmp[i] == '\r') break;
            if (tmp[i] < '0' || tmp[i] > '9') throw InvalidExp();
            sum = tmp[i] - '0' + 10 * sum;
        }
        return sum;
    }

    static bool checkPw(const std::string &tmp) {
        if (tmp.empty() || tmp.size() > 31 || (tmp.size() == 31 && tmp[30] != '\r')) return false;
        else return true;
    }

    static bool checkUser(const std::string &tmp) {
        if (tmp.empty() || tmp.size() > 21 || (tmp.size() == 21 && tmp[20] != '\r')) return false;
        for (int i = 0; i < tmp.size(); i++) {
            if (tmp[i] < '0' || tmp[i] > '9') {
                if (tmp[i] < 'a' || tmp[i] > 'z') {
                    if (tmp[i] < 'A' || tmp[i] > 'Z') {
                        if (tmp[i] != '_' && tmp[i] != '\r') return false;
                    }
                }
            }
        }
        return true;
    }

    static bool checkMail(const std::string &str) {
        if (str.empty() || str.size() > 31 || (str.size() == 31 && str[30] != '\r')) return false;
        for (int i = 0; i < str.size(); i++) {
            if (str[i] < '0' || str[i] > '9') {
                if (str[i] < 'a' || str[i] > 'z') {
                    if (str[i] < 'A' || str[i] > 'Z') {
                        if (str[i] != '@' && str[i] != '.' && str[i] != '\r') return false;
                    }
                }
            }
        }
        return true;
    }

    static int stringToStamp(const std::string &str) {
        int sum = 0;
        for (int i = 1; i <= str.size() - 2; i++) {
            sum = (str[i] - '0') + sum * 10;
        }
        return sum;
    }

    static bool checkPri(int num) {
        if (num == -1) return false;
        if (num > 10) return false;
        return true;
    }

    static Date stringToDate(const std::string &tmp) {
        std::string temp;
        Date date;
        int i = 0;
        for (; i < tmp.size(); i++) {
            if (tmp[i] == '-') {
                date.st.month = StringToInteger(temp);
                temp = "";
                continue;
            }
            if (tmp[i] == '|') {
                date.st.day = StringToInteger(temp);
                temp = "";
                break;
            }
            temp += tmp[i];
        }
        i++;
        for (; i < tmp.size(); i++) {
            if (tmp[i] == '-') {
                date.en.month = StringToInteger(temp);
                temp = "";
                continue;
            }
            temp += tmp[i];
        }
        date.en.day = StringToInteger(temp);
        return date;
    }

    static Yuki::vector<std::string> SplitWord(const std::string &tmp) {
        Yuki::vector<std::string> all;
        std::string t;
        for (int i = 0; i <= tmp.size(); i++) {
            if (tmp[i] == '|' || i == tmp.size()) {
                all.push_back(t);
                t = "";
                continue;
            }
            t += tmp[i];
        }
        return all;
    }

    static Yuki::vector<int> SpiltInt(const std::string &tmp) {
        Yuki::vector<int> all;
        std::string t;
        for (int i = 0; i <= tmp.size(); i++) {
            if (tmp[i] == '|' || i == tmp.size()) {
                all.push_back(StringToInteger(t));
                t = "";
                continue;
            }
            t += tmp[i];
        }
        if (all.empty()) throw InvalidExp();
        return all;
    }

    static Time stringToTime(const std::string &tmp) {
        Time time;
        std::string t;
        for (int i = 0; i < tmp.size(); i++) {
            if (tmp[i] == ':') {
                time.hour = StringToInteger(t);
                t = "";
                continue;
            }
            t += tmp[i];
        }
        time.minute = StringToInteger(t);
        if (!Time::checkTime(time)) throw InvalidExp();
        return time;
    }

    static bool checkTrain(std::string &tmp) {
        if (tmp.size() > 20 || tmp.empty()) return false;
        for (int i = 0; i < tmp.size(); i++) {
            if (tmp[i] < '0' || tmp[i] > '9') {
                if (tmp[i] < 'a' || tmp[i] > 'z') {
                    if (tmp[i] < 'A' || tmp[i] > 'Z') {
                        if (tmp[i] != '_' && tmp[i] != '\r') return false;
                    }
                }
            }
        }
        return true;
    }
    static Day stringToDay(const std::string &tmp) {
        if (tmp.size() > 5) throw InvalidExp();
        std::string temp;
        Day day;
        for (int i = 0; i < tmp.size(); i++) {
            if (tmp[i] == '-') {
                day.month = StringToInteger(temp);
                temp = "";
                continue;
            }
            temp += tmp[i];
        }
        day.day = StringToInteger(temp);
        if (!Day::checkDay(day)) throw InvalidExp();
        return day;
    }
};

#endif //TICKETSYSTEM_TOKENSCANNER_HPP
