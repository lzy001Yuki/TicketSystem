#ifndef TICKETSYSTEM_HASHMAP_HPP
#define TICKETSYSTEM_HASHMAP_HPP
#include<iostream>
#include"FileSystem.hpp"
#include"queue.hpp"
#include "utility.hpp"
// 开散列表 Buffer Pool
// 储存内容:Key--pair<T1, T2> Value--pos(pair在叶子中的位置)
// 插入一个数字就插入BufferPool里面，如果满了就Pop
namespace Yuki {
    template <class Key, class Value, class Function, int CacheSize, int max_size>
    class HashMap {
    private:
        // HashMap里储存每一个HashNode的指针，防止没有默认函数的情况出现
        class HashNode {
            friend class HashMap;

        private:
            Key key;
            Value value;
        public:
            HashNode() = default;

            explicit HashNode(const Key &key_, const Value &value_) : key(key_) {
                value = value_;
            }

            ~HashNode() = default;
        };

        typedef HashNode node;
        node **array = nullptr;
        //Yuki::queue <Key> q;
        Function fun;
        int size = 0;
    public:
        HashMap() {
            array = new node *[max_size];// 储存每一条链表的头结点
            for (int i = 0; i < max_size; i++) {
                array[i] = nullptr;
            }
            size = 0;
        }

        ~HashMap() {
            //clear();
            delete[] array;
        }

        void clear() {
            for (int i = 0; i < max_size; i++) {
                node *del = array[i];
                if (del == nullptr) continue;
                node *nxt = del->next;
                while (nxt != nullptr) {
                    delete del;
                    del = nxt;
                    nxt = nxt->next;
                }
                array[i] = nullptr;
            }
        }

        // 判断是否重复插入
        // spilt node
        // 如果存在，覆盖以前的插入
        // true -- bpt false -- file
        void insert(const Key &key, const Value &value, FileSystem<Value> &file, int info_len, bool flag) {
            int pos = fun(key) % max_size;
            node *all = array[pos];
            if (all == nullptr) {
                array[pos] = new node(key, value);
                size++;
                return;
            } else {
                if (all->key == key) all->value = value;
                else {
                    if (flag) file.write(all->value, info_len * sizeof(int) + (all->key + 1) * sizeof(Value));
                    else file.write(all->value, info_len * sizeof(int) + all->key * sizeof(Value));
                    all->key = key;
                    all->value = value;
                }
                return;
            }
        }

        // merge node
        void erase(const Key &key) {
            int pos = fun(key) % max_size;
            node *all = array[pos];
            // 不存在
            if (all == nullptr) return;
            delete all;
            array[pos] = nullptr;
        }

        // 似乎不用考虑value有没有默认构造函数
        bool find(const Key &key, Value &value) {
            int pos = fun(key) % max_size;
            node *all = array[pos];
            if (all != nullptr && all->key == key) {
                value = all->value;
                return true;
            }
            return false;
        }

        int size_() { return size; }

        /* Value pop(Key &num) {
             int i = 0;
             for (; i < max_size; i++) {
                 if (array[i] != nullptr) break;
             }
             num = array[i]->key;
             Value v = array[i]->value;
             node *nxt = array[i]->next;
             node *tmp = array[i];
             delete tmp;
             array[i] = nxt;
             return v;
         }*/
        void clear_(FileSystem<Value> &file, int info_len) {
            for (int i = 0; i < max_size; i++) {
                node *tmp = array[i];
                if (tmp == nullptr) continue;
                file.write(tmp->value, info_len * sizeof(int) + sizeof(Value) * (tmp->key + 1));
            }
        }

        void clearing(FileSystem<Value> &file, int info_len) {
            for (int i = 0; i < max_size; i++) {
                node *tmp = array[i];
                if (tmp == nullptr) continue;
                file.write(tmp->value, info_len * sizeof(int) + sizeof(Value) * (tmp->key));
            }
        }

    };
}
#endif //TICKETSYSTEM_HASHMAP_HPP
