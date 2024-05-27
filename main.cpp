#include <iostream>
#include<string>
#include "src/UserSystem.hpp"
#include "src/TrainSystem.hpp"
#include"src/TicketSystem.hpp"
#include "src/TokenScanner.hpp"


bool processLine(const std::string &line, UserManagement &userSystem, TrainSystem &trainSystem, TicketSystem &ticketSystem) {
    Yuki::vector<std::string> parse;
    TokenScanner::SplitString(line, parse);
    if (parse.empty()) return true;
    std::string timeStamp = parse[0];
    //if (parse.size() == 1) throw InvalidExp();
    std::string cmd = parse[1];
    std::cout<<timeStamp<<' ';

    // 检查query_transfer的问题 余票也有问题
    if (timeStamp == "[29393]") {
        int y = 2;
    }

    if (cmd == "add_user") {
        myChar<24> cur_name, name, user;
        myChar<32> pw, mail;
        int pri = -1;
        int pos = 2;
        while (pos < parse.size()) {
            if (parse[pos] == "-c") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                cur_name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-u") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                user.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-p") {
                //if (!TokenScanner::checkPw(parse[pos + 1])) throw InvalidExp();
                pw.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-n") {
                name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-m") {
                //if (!TokenScanner::checkMail(parse[pos + 1])) throw InvalidExp();
                mail.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-g") {
                pri = TokenScanner::StringToInteger(parse[pos + 1]);
                //if (!TokenScanner::checkPri(pri)) throw InvalidExp();
            }
            pos += 2;
        }
        std::cout<<userSystem.addUser(cur_name, user, pw, name, mail, pri)<<'\n';
    } else if (cmd == "login") {
        //if (parse.size() != 6) throw InvalidExp();
        myChar<24> name;
        myChar<32> pw;
        int pos = 2;
        while (pos < parse.size()) {
            if (parse[pos] == "-u") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-p") {
                //if (!TokenScanner::checkPw(parse[pos + 1])) throw InvalidExp();
                pw.toChar(parse[pos + 1]);
            }
            pos += 2;
        }
        std::cout<<userSystem.logIn(name, pw)<<'\n';
    } else if (cmd == "logout") {
        //if (parse.size() != 4) throw InvalidExp();
        myChar<24> name;
        //if (!TokenScanner::checkUser(parse[3])) throw InvalidExp();
        name.toChar(parse[3]);
        std::cout<<userSystem.logOut(name)<<'\n';
    } else if (cmd == "query_profile") {
        //if (parse.size() != 6) throw InvalidExp();
        int pos = 2;
        myChar<24> name, user;
        while (pos < parse.size()) {
            if (parse[pos] == "-c") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-u") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                user.toChar(parse[pos + 1]);
            }
            pos += 2;
        }
        Yuki::pair<UserInfo, bool> ans = userSystem.query_profile(name, user);
        if (!ans.second) std::cout<<"-1\n";
        else std::cout<<ans.first;
    } else if (cmd == "modify_profile") {
        myChar<24> cur_name, user, name;
        myChar<32> pw, mail;
        int pri = -1;
        int pos = 2;
        while (pos < parse.size()) {
            if (parse[pos] == "-c") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                cur_name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-u") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                user.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-p") {
                //if (!TokenScanner::checkPw(parse[pos + 1])) throw InvalidExp();
                pw.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-n") {
                name.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-m") {
                //if (!TokenScanner::checkMail(parse[pos + 1])) throw InvalidExp();
                mail.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-g") {
                pri = TokenScanner::StringToInteger(parse[pos + 1]);
                //if (!TokenScanner::checkPri(pri)) throw InvalidExp();
            }
            pos += 2;
        }
        Yuki::pair<UserInfo, bool> ans = userSystem.modify_profile(cur_name, user, pw, name, mail, pri);
        if (!ans.second) std::cout<<"-1\n";
        else std::cout<<ans.first;
    } else if (cmd == "add_train") {
        myChar<24> id;
        int n;
        int m;
        Yuki::vector<string> s;
        Time ini_time;
        Yuki::vector<int> p;
        Yuki::vector<int> t;
        Yuki::vector<int> o;
        Date date;
        myChar<2> type;
        int pos = 2;
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        while (pos < parse.size()) {
            if (parse[pos] == "-i") {
                //if (!TokenScanner::checkTrain(parse[pos + 1])) throw InvalidExp();
                id.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-n") {
                n = TokenScanner::StringToInteger(parse[pos + 1]);
                //if (n > 100) throw InvalidExp();
            }
            if (parse[pos] == "-m") {
                m = TokenScanner::StringToInteger(parse[pos + 1]);
                //if (m > 100000) throw InvalidExp();
            }
            if (parse[pos] == "-s") {
                s = TokenScanner::SplitWord(parse[pos + 1]);
            }
            if (parse[pos] == "-p") {
                p = TokenScanner::SpiltInt(parse[pos + 1]);
            }
            if (parse[pos] == "-x") {
                ini_time = TokenScanner::stringToTime(parse[pos + 1]);
            }
            if (parse[pos] == "-t") {
                t = TokenScanner::SpiltInt(parse[pos + 1]);
            }
            if (parse[pos] == "-o") {
                o = TokenScanner::SpiltInt(parse[pos + 1]);
            }
            if (parse[pos] == "-d") {
                date = TokenScanner::stringToDate(parse[pos + 1]);
            }
            if (parse[pos] == "-y") {
                type.toChar(parse[pos + 1]);
            }
            pos += 2;
        }
        std::cout<<trainSystem.add_train(id, n, m, s, p, ini_time, t, o, date, type)<<'\n';
    } else if (cmd == "query_train") {
        //if (parse.size() != 6) throw InvalidExp();
        myChar<24> id;
        Day day;
        int pos = 2;
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        while (pos < parse.size()) {
            if (parse[pos] == "-i") {
                //if (!TokenScanner::checkTrain(parse[pos + 1])) throw InvalidExp();
                id.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-d") {
                day = TokenScanner::stringToDay(parse[pos + 1]);
            }
            pos += 2;
        }
        Yuki::pair<TrainInfo, bool> ans = trainSystem.query_train(id, day);
        if (!ans.second) std::cout<<"-1\n";
        else {
            trainSystem.printTrain(day, ans.first);
        }
    } else if (cmd == "delete_train") {
        //if (parse.size() != 4) throw InvalidExp();
        //if (parse[2] != "-i") throw InvalidExp();
        myChar<24> id;
        id.toChar(parse[3]);
        std::cout<<trainSystem.delete_train(id)<<'\n';
    } else if (cmd == "release_train") {
        //if (parse.size() != 4) throw InvalidExp();
        //if (parse[2] != "-i") throw InvalidExp();
        myChar<24> id;
        id.toChar(parse[3]);
        std::cout<<trainSystem.release_train(id)<<'\n';
    } else if (cmd == "query_ticket" || cmd == "query_transfer") {
        myChar<30> st, en;
        Day day;
        bool flag;
        int pos = 2;
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        while (pos < parse.size()) {
            if (parse[pos] == "-s") st.toChar(parse[pos + 1]);
            if (parse[pos] == "-t") en.toChar(parse[pos + 1]);
            if (parse[pos] == "-d") {
                day = TokenScanner::stringToDay(parse[pos + 1]);
            }
            if (parse[pos] == "-p") {
                if (parse[pos + 1] == "time") flag = true;
                else if (parse[pos + 1] == "cost") flag = false;
                //else throw InvalidExp();
            }
            pos += 2;
        }
        if (cmd == "query_ticket") trainSystem.query_ticket(st, en, day, flag);
        else trainSystem.query_transfer(st, en, day, flag);
    } else if (cmd == "buy_ticket"){
        myChar<24> u, id;
        Day day;
        myChar<30> st, en;
        int n;
        bool flag = false;
        int pos = 2;
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        while (pos < parse.size()) {
            if (parse[pos] == "-u") u.toChar(parse[pos + 1]);
            if (parse[pos] == "-i") id.toChar(parse[pos + 1]);
            if (parse[pos] == "-d") day = TokenScanner::stringToDay(parse[pos + 1]);
            if (parse[pos] == "-n") n = TokenScanner::StringToInteger(parse[pos + 1]);
            if (parse[pos] == "-f") st.toChar(parse[pos + 1]);
            if (parse[pos] == "-t") en.toChar(parse[pos + 1]);
            if (parse[pos] == "-q") {
                if (parse[pos + 1] == "false") flag = false;
                else flag = true;
            }
            pos += 2;
        }
        ticketSystem.buy_ticket(u, id, day, n, st, en, flag, userSystem, trainSystem, TokenScanner::stringToStamp(parse[0]));
    } else if (cmd == "query_order") {
        //if (parse.size() != 4) throw InvalidExp();
        myChar<24> u;
        //if (parse[2] != "-u") throw InvalidExp();
        //if (!TokenScanner::checkUser(parse[3])) throw InvalidExp();
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        u.toChar(parse[3]);
        ticketSystem.query_order(u, userSystem, trainSystem);
    } else if (cmd == "refund_ticket") {
        int n;
        myChar<24> u;
        if (parse.size() == 4) n = 1;
        //if (parse.size() != 4 && parse.size() != 6) throw InvalidExp();
        int pos = 2;
        //parse[parse.size() - 1] = parse[parse.size() - 1].substr(0, parse[parse.size() - 1].size() - 1);
        while (pos < parse.size()) {
            if (parse[pos] == "-u") {
                //if (!TokenScanner::checkUser(parse[pos + 1])) throw InvalidExp();
                u.toChar(parse[pos + 1]);
            }
            if (parse[pos] == "-n") {
                n = TokenScanner::StringToInteger(parse[pos + 1]);
            }
            pos += 2;
        }
        std::cout<<ticketSystem.refund_ticket(u, n, userSystem, trainSystem)<<'\n';
    } else if (cmd == "clean") {
        TrainSystem::clean();
        TicketSystem::clean();
        UserManagement::clean();
        std::cout<<"0\n";
    } else if (cmd == "exit") {
        std::cout<<"bye\n";
        return false;
    }
    return true;
}

int main() {
    //std::freopen("../testcases/pressure_1_easy/46.in", "r", stdin);
    //std::freopen("answer.txt", "w", stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    UserManagement userManagement;
    TrainSystem trainSystem;
    TicketSystem ticketSystem;
    while (true) {
        std::string line;
        getline(std::cin, line);
        try {
            if (!processLine(line, userManagement, trainSystem, ticketSystem)) break;
        } catch (InvalidExp &error) {
            std::cout << error.what();
        }
    }
    return 0;
}


