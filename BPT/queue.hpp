#ifndef TICKETSYSTEM_QUEUE_HPP
#define TICKETSYSTEM_QUEUE_HPP

#include <memory>
#include <cstddef>
namespace Yuki {
    template<class T> class queue;
    template<class T>
    class node{
        friend class queue<T>;
    private:
        T key;
        node* next = nullptr;
    public:
        node() = default;
        explicit node(const T &key_) :key(key_), next(nullptr){}
        ~node() = default;
    };

    // 双向链表只用储存index_num， 然后通过hashMap去查找键值
    template <class T>
    class queue{
    private:
        node<T>* head = nullptr;
        node<T>* tail = nullptr;
        int size = 0;
    public:
        queue() {
            head = new node<T>;
            tail = head;
        }
        ~queue() {
            clear();
            delete head;
        }

        void insert(const T &value) {
            tail->next = new node(value);
            tail = tail->next;
            size++;
        }

        // 使用front之前应该检查是否为空
        T front() {
            if (head->next != nullptr) {
                T tmp = head->next->key;
                node<T>* del = head->next;
                head->next = del->next;
                if (del->next == nullptr) tail = head;
                delete del;
                del = nullptr;
                size--;
                return tmp;
            }
        }

        bool empty() {
            if (size == 0) return true;
            else return false;
        }

        void erase(const T &value) {
            node<T> *cur = head;
            while (cur->next != nullptr) {
                if (cur->next->key == value) break;
                cur = cur->next;
            }
            node<T>* del = cur->next;
            if (del == nullptr) return;
            cur->next = del->next;
            if (del->next == nullptr) tail = cur;
            delete del;
            del = nullptr;
            size--;
        }

        void clear() {
            while (head->next != nullptr) {
                node<T>* nextDel = head->next->next;
                delete head->next;
                head->next = nullptr;
                head->next = nextDel;
            }
        }
    };
}


#endif //TICKETSYSTEM_QUEUE_HPP
