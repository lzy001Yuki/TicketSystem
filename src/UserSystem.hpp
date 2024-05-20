#ifndef TICKETSYSTEM_USERSYSTEM_HPP
#define TICKETSYSTEM_USERSYSTEM_HPP
#include"../BPT/BPTree.hpp"
#include<filesystem>

class UserManagement;
class UserInfo{
    friend class UserManagement;
    friend class TicketSystem;
private:
    char username[24] = {'\0'};
    char password[32] = {'\0'};
    char name[20] = {'\0'};
    char mailAddr[32] = {'\0'};
    int privilege = -1;
    bool isLogin = false;
public:
    UserInfo() = default;
    explicit UserInfo(const char* un, const char *pw, const char* n, const char* ma, int p, bool flag = false) :privilege(p)
            , isLogin(flag){
        strcpy(username, un);
        strcpy(password, pw);
        strcpy(name, n);
        strcpy(mailAddr, ma);
    }

    void create(const char* un, const char *pw, const char* n, const char* ma, int p, bool flag = false) {
        strcpy(username, un);
        strcpy(password, pw);
        strcpy(name, n);
        strcpy(mailAddr, ma);
        privilege = p;
        isLogin = flag;
    }
    friend std::ostream &operator<<(std::ostream &os, const UserInfo &user_info) {
        os<<user_info.username<<' '<<user_info.name<<' '<<user_info.mailAddr<<' '<<user_info.privilege<<'\n';
        return os;
    }

    bool operator == (const UserInfo& other) {
        return !(strcmp(username, other.username));
    }

    bool operator < (const UserInfo& other) {
        if (strcmp(username, other.username) < 0) return true;
        else return false;
    }

    bool operator > (const UserInfo &other) {
        if (strcmp(username, other.username) > 0) return true;
        else return false;
    }

    UserInfo& operator=(const UserInfo & other) {
        if (this == &other) return *this;
        strcpy(username, other.username);
        strcpy(password, other.password);
        strcpy(name, other.name);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;
        isLogin = other.isLogin;
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

class UserManagement{
    friend class TicketSystem;
private:
    // 键值对为username--user_info
    BPT<char, int, nameFunction, 22, 2, 2> userData;
    FileSystem<UserInfo, 2> userIndex;
    int total = 0;

    static ll changeToPos(int index) {
        return 2 * sizeof(int) + index * sizeof(UserInfo);
    }
public:
    UserManagement(): userData("user.txt", "space_user.txt"){
        userIndex.initialise("userIndex.txt");
        userIndex.get_info(total, 1);
    }
    ~UserManagement() {
        userIndex.write_info(total, 1);
    }
    int addUser(const char* cur_user, const char *new_user, const char *pw, const char *name, const char *mail, int p) {
        if (userData.empty()) {
            UserInfo rootInfo(new_user, pw, name, mail, 10);
            userData.insert(Yuki::pair<char, int>(new_user, total));
            userIndex.write(rootInfo, changeToPos(total));
            UserInfo other;
            userIndex.read(other, changeToPos(total));
            total++;
            return 0;
        }
        if (cur_user == nullptr) return -1;
        UserInfo cur_info;
        int info_index;
        bool isHere = userData.findKV(cur_user, info_index);
        if (!isHere) return -1; // cur_name 不存在
        userIndex.read(cur_info, changeToPos(info_index));
        if (!cur_info.isLogin) return -1; // 当前用户未登录
        if (p > cur_info.privilege) return -1; // 创建用户权限大于当前用户
        if (p == cur_info.privilege && strcmp(cur_user, new_user) != 0) return -1;
        UserInfo user_info(new_user, pw, name, mail, p);
        int user_index;
        bool exist = userData.findKV(new_user, user_index);
        if (exist) {
            return -1;
        }
        userData.insert(Yuki::pair<char, int> (new_user, total));
        userIndex.write(user_info, changeToPos(total));
        total++;
        return 0;
    }

    int logIn(const char *username, const char *password) {
        UserInfo now_user;
        int now_index;
        bool exist = userData.findKV(username, now_index);
        if (!exist) return -1;
        userIndex.read(now_user, changeToPos(now_index));
        if (now_user.isLogin) return -1;
        if (strcmp(password, now_user.password) != 0) return -1;
        now_user.isLogin = true;
        userIndex.write(now_user, changeToPos(now_index));
        return 0;
    }

    int logOut(const char *username) {
        UserInfo now_user;
        int now_index;
        bool exist = userData.findKV(username, now_index);
        if (!exist) return -1;
        userIndex.read(now_user, changeToPos(now_index));
        if (!now_user.isLogin) return -1;
        now_user.isLogin = false;
        userIndex.write(now_user, changeToPos(now_index));
        return 0;
    }

    Yuki::pair<UserInfo, bool> query_profile(const char* cur_name, const char *username) {
        UserInfo cur_user;
        int cur_index;
        bool exist = userData.findKV(cur_name, cur_index);
        if (!exist) return {cur_user, false};
        userIndex.read(cur_user, changeToPos(cur_index));
        if (!cur_user.isLogin) return {cur_user, false};
        UserInfo query_user;
        int q_index;
        bool exist_ = userData.findKV(username, q_index);
        if (!exist_) return {query_user, false};
        userIndex.read(query_user, changeToPos(q_index));
        if (cur_user.privilege < query_user.privilege) return {query_user, false};
        if (cur_user.privilege == query_user.privilege && strcmp(cur_name, username) != 0) return {query_user, false};
        return {query_user, true};
    }

    Yuki::pair<UserInfo, bool> modify_profile(const char* cur_name, const char *username, const char *pw, const char *n, const char *mail, int p = -1) {
        UserInfo cur_user;
        int cur_index;
        bool exist = userData.findKV(cur_name, cur_index);
        if (!exist) return {cur_user, false};
        userIndex.read(cur_user, changeToPos(cur_index));
        if (!cur_user.isLogin) return {cur_user, false};
        UserInfo query_user;
        int q_index;
        bool exist_ = userData.findKV(username, q_index);
        if (!exist_) return {query_user, false};
        userIndex.read(query_user, changeToPos(q_index));
        if (query_user.privilege > cur_user.privilege) return {cur_user, false};
        if (query_user.privilege == cur_user.privilege && ((strcmp(cur_name, username) != 0) || p >= query_user.privilege) ) return {cur_user, false};
        if (pw[0] != '\0') strcpy(query_user.password, pw);
        if (n[0] != '\0') strcpy(query_user.name, n);
        if (mail[0] != '\0') strcpy(query_user.mailAddr, mail);
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
    void LogTraverse() {
        for (int i = 0; i < total; i++) {
            UserInfo user;
            userIndex.read(user, changeToPos(i));
            if (user.isLogin) {
                user.isLogin = false;
                userIndex.write(user, changeToPos(i));
            }
        }
    }

};

#endif //TICKETSYSTEM_USERSYSTEM_HPP
