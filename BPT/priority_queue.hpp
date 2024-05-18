#ifndef TICKETSYSTEM_PRIORITY_QUEUE_HPP
#define TICKETSYSTEM_PRIORITY_QUEUE_HPP
#include <cstddef>
#include <functional>

namespace Yuki {

/**
 * a container like std::priority_queue which is a heap internal.
 */

    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:
        size_t capacity = 0;
        Compare comp;
        class node{
            friend class priority_queue<T>;
        public:
            T *data = nullptr;
            node *lchild;
            node *rchild;
            int dist;
            node() {
                dist = 0;
                lchild = rchild = nullptr;
                data = nullptr;
            }

            explicit node(const T &value) {
                lchild = rchild = nullptr;
                dist = 0;
                data = new T(value);
            }
            ~node() {delete data;}

        };
        node *root = nullptr;

        int getDistance(node *obj) {
            return obj->dist;
        }

        node* Merge(node *&lhs, node *&rhs) {
            if (lhs == nullptr) return rhs;
            if (rhs == nullptr) return lhs;
            const T l = *(lhs->data);
            const T r = *(rhs->data);
            try {
                comp(l, r);
            } catch(...) {
                delete rhs;
            }

            if (comp(l, r)) {
                // 根节点较大的堆是lhs
                // 归并lhs rhs->rchild，结果给rhs->rchild
                rhs->rchild = Merge(lhs, rhs->rchild);
                if (rhs->lchild != nullptr && rhs->rchild != nullptr) {
                    if (getDistance(rhs->lchild) < getDistance(rhs->rchild)) {
                        node *tmp = rhs->lchild;
                        rhs->lchild = rhs->rchild;
                        rhs->rchild = tmp;
                    }
                } else {
                    if (rhs->lchild == nullptr) {
                        node *tmp = rhs->lchild;
                        rhs->lchild = rhs->rchild;
                        rhs->rchild = tmp;
                    }
                }
                if (rhs->rchild != nullptr) rhs->dist = rhs->rchild->dist + 1;
                else rhs->dist = 0;
                return rhs;
            } else {
                lhs->rchild = Merge(rhs, lhs->rchild);
                if (lhs->lchild != nullptr && lhs->rchild != nullptr) {
                    if (getDistance(lhs->lchild) < getDistance(lhs->rchild)) {
                        // 交换左右子树
                        node *tmp = lhs->lchild;
                        lhs->lchild = lhs->rchild;
                        lhs->rchild = tmp;
                    }
                } else {
                    if (lhs->lchild == nullptr) {
                        node *tmp = lhs->lchild;
                        lhs->lchild = lhs->rchild;
                        lhs->rchild = tmp;
                    }
                }
                if (lhs->rchild != nullptr) lhs->dist = lhs->rchild->dist + 1;
                else lhs->dist = 0;
                return lhs;
            }
        }
    public:
        /**
         * TODO constructors
         */
        priority_queue() {
            capacity = 0;
            root = nullptr;
        }
        priority_queue(const priority_queue &other) {
            capacity = other.capacity;
            comp = other.comp;
            if (other.root != nullptr) {
                root = new node(*(other.root->data));
                copy(root, other.root);
            }
        }
        void copy(node *obj, const node *other) {
            if (other == nullptr) return;
            obj->lchild = other->lchild ? new node(*(other->lchild->data)) : nullptr;
            obj->rchild = other->rchild ? new node(*(other->rchild->data)) : nullptr;
            copy(obj->lchild, other->lchild);
            copy(obj->rchild, other->rchild);
        }

        /**
         * TODO deconstructor
         */
        ~priority_queue() {
            clear();
        }
        /**
         * TODO Assignment operator
         */
        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) return *this;
            capacity = other.capacity;
            comp = other.comp;
            clear();
            if (other.root != nullptr) {
                root = new node(*(other.root->data));
                copy(root, other.root);
            }
            return *this;
        }
        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T & top() const {
            if (!empty()) return (*root->data);
        }
        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T &e) {
            node *newNode = new node(e);
            root = Merge(root, newNode);
            capacity++;
        }
        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (!empty()) {
                capacity--;
                node *oldOne = root;
                root = Merge(root->lchild, root->rchild);
                delete oldOne;
            }
        }
        /**
         * return the number of the elements.
         */

        void clear() {
            clear_(root);
        }

        void clear_(node *tmp) {
            if (tmp == nullptr) return;
            clear_(tmp->lchild);
            tmp->lchild = nullptr;
            clear_(tmp->rchild);
            tmp->rchild = nullptr;
            delete tmp;
        }

        size_t size() const {
            return capacity;
        }
        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            if (!capacity) return true;
            else return false;
        }
        /**
         * merge two priority_queues with at least O(logn) complexity.
         * clear the other priority_queue.
         */
        void merge(priority_queue &other) {
            root = Merge(root, other.root);
            capacity += other.capacity;
            other.capacity = 0;
            other.root = nullptr;
        }
    };

}
#endif //TICKETSYSTEM_PRIORITY_QUEUE_HPP
