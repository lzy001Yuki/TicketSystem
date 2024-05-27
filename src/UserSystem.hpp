#ifndef TICKETSYSTEM_USERSYSTEM_HPP
#define TICKETSYSTEM_USERSYSTEM_HPP
#include"../BPT/BPTree.hpp"
#include<filesystem>
#include"../BPT/map.hpp"
#include"userType.hpp"
class UserManagement{
    friend class TicketSystem;
private:
    // 键值对为username--user_info
    BPT<myChar<24>, int, nameFunction, 22, 2, 1024> userData;
    FileSystem<UserInfo, 2> userIndex;
    int total = 0;
    static ll changeToPos(int index) {
        return 2 * sizeof(int) + index * sizeof(UserInfo);
    }
    sjtu::map<myChar<24>, int> LogIn;
public:
    UserManagement(): userData("user.txt", "space_user.txt"){
        userIndex.initialise("userIndex.txt");
        userIndex.get_info(total, 1);
    }
    ~UserManagement() {
        userIndex.write_info(total, 1);
    }
    int addUser(const myChar<24>& cur_user, const myChar<24>& new_user, const myChar<32>& pw, const myChar<24>&  name, const myChar<32>& mail, int p) {
        if (userData.empty()) {
            UserInfo rootInfo(new_user, pw, name, mail, 10);
            userData.insert(Yuki::pair<myChar<24> , int>(new_user, total));
            userIndex.write(rootInfo, changeToPos(total));
            total++;
            return 0;
        }
        if (cur_user.isNull()) return -1;
        auto it = LogIn.find(cur_user);
        if (it == LogIn.end()) return -1;
        if (p > it->second) return -1; // 创建用户权限大于当前用户
        if (p == it->second && cur_user != new_user) return -1;
        UserInfo user_info(new_user, pw, name, mail, p);
        int user_index;
        bool exist = userData.findKV(new_user, user_index);
        if (exist) {
            return -1;
        }
        userData.insert(Yuki::pair<myChar<24>, int> (new_user, total));
        userIndex.write(user_info, changeToPos(total));
        total++;
        return 0;
    }

    int logIn(const myChar<24>& username, const myChar<32>& password) {
        UserInfo now_user;
        int now_index;
        bool exist = userData.findKV(username, now_index);
        if (!exist) return -1;
        auto it = LogIn.find(username);
        if (it != LogIn.end()) return -1;
        userIndex.read(now_user, changeToPos(now_index));
        if (password != now_user.password) return -1;
        LogIn.insert(Yuki::pair<myChar<24>, int>(username, now_user.privilege));
        return 0;
    }

    int logOut(const myChar<24>& username) {
        auto it = LogIn.find(username);
        if (it == LogIn.end()) return -1;
        LogIn.erase(it);
        return 0;
    }

    Yuki::pair<UserInfo, bool> query_profile(const myChar<24>& cur_name, const myChar<24>& username) {
        UserInfo query_user;
        auto it = LogIn.find(cur_name);
        if (it == LogIn.end()) return {query_user, false};
        int q_index;
        bool exist_ = userData.findKV(username, q_index);
        if (!exist_) return {query_user, false};
        userIndex.read(query_user, changeToPos(q_index));
        if (it->second < query_user.privilege) return {query_user, false};
        if (it->second == query_user.privilege && cur_name != username) return {query_user, false};
        return {query_user, true};
    }

    Yuki::pair<UserInfo, bool> modify_profile(const myChar<24>& cur_name, const myChar<24>& username, const myChar<32>& pw, const myChar<24>& n, const myChar<32>& mail, int p = -1) {
        UserInfo query_user;
        auto it = LogIn.find(cur_name);
        if (it == LogIn.end()) return {query_user, false};
        int q_index;
        bool exist_ = userData.findKV(username, q_index);
        if (!exist_) return {query_user, false};
        userIndex.read(query_user, changeToPos(q_index));
        if (query_user.privilege > it->second) return {query_user, false};
        if (query_user.privilege == it->second && ((cur_name != username) || p >= query_user.privilege)) return {query_user, false};
        if (!pw.isNull()) query_user.password = pw;
        if (!n.isNull()) query_user.name = n;
        if (!mail.isNull()) query_user.mailAddr = mail;
        if (p != -1) query_user.privilege = p;
        userIndex.write(query_user, changeToPos(q_index));
        return {query_user, true};
    }
    static void clean() {
        std::filesystem::path path1("user.txt");
        std::filesystem::path path2("space_user.txt");
        if (std::filesystem::exists(path1)) std::filesystem::remove(path1);
        if (std::filesystem::exists(path2)) std::filesystem::remove(path2);
    }
};

#endif //TICKETSYSTEM_USERSYSTEM_HPP