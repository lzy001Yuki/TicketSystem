#ifndef TICKETSYSTEM_FILESYSTEM_HPP
#define TICKETSYSTEM_FILESYSTEM_HPP
#include<fstream>
#include<string>
#include<unistd.h>
using std::fstream;
using std::string;

// Data是一个键值对
template<class Data, int info_len = 2>
class FileSystem{
private:
    string file_name;
public:
    fstream file;
    explicit FileSystem(const string &name = "") {
        file_name = name;
    }

    void initialise(const string &FN) {
        if (FN != "") file_name = FN;
        if (access(file_name.c_str(), F_OK) == 0) {
            return;
        }
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // 读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len)
            return;
        file.open(file_name);
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        return;
    }

    // 将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len)
            return;
        file.open(file_name, std::fstream::in | std::fstream::out);
        file.seekp((n - 1) * sizeof(int), std::fstream::beg);
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // 在文件合适位置写入类对象t，并返回写入的位置索引index
    // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    // 位置索引index可以取为对象写入的起始位置
    void write(Data &t, long long place, int size = 1) {
        file.open(file_name ,std::fstream::in | std::fstream::out);
        file.seekp(place);
        file.write(reinterpret_cast<char *>(&t), sizeof(Data) * size);
        file.close();
    }
    void read(Data &t, const long long index, int size = 1) {
        file.open(file_name);
        file.seekg(index);
        file.read(reinterpret_cast<char *>(&t), sizeof(Data) * size);
        file.close();
        return;
    }

    void clear() {
        file.open(file_name);
        file.clear();
        file.close();
    }
};


#endif //TICKETSYSTEM_FILESYSTEM_HPP
