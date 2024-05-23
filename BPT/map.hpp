#ifndef TICKETSYSTEM_MAP_HPP
#define TICKETSYSTEM_MAP_HPP
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include"exceptions.hpp"

namespace sjtu {
    template<class T>
    class node{
    public:
        T valueType;
        bool color = false; // false--red true--black
        node<T>* left = nullptr;
        node<T>* right = nullptr;
        node<T>* parent = nullptr;

        node() : color(false), left(nullptr), right(nullptr), parent(nullptr){}
        explicit node(const T &value, bool color_ = false) : valueType(value), color(color_){}
        bool operator==(const node& other){
            return valueType == other.valueType;
        }
        bool operator!=(const node& other) {
            return valueType != other.valueType;
        }
        ~node() = default;
    };


    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {

    public:
        typedef Yuki::pair<const Key, T> value_type;
        typedef node<value_type> map_node;

        size_t length = 0;
        map_node* root = nullptr;
        Compare key_compare;
        map_node* head = nullptr;
        std::allocator<map_node> alloc;
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use Yuki::map as value_type by typedef.
         */

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;
        class iterator {
            friend class map;
            friend class const_iterator;
        private:
            //map_node* it;
            Compare key_compare;
        public:
            map_node* it;
            iterator() {
                it = nullptr;
            }
            explicit iterator(map_node* obj, bool isEnd_ = false) {
                it = obj;
            }
            iterator(const iterator &other) {
                it = other.it;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp(it);
                if (it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent->parent != nullptr) {
                    if (key_compare(it->valueType.first, it->parent->valueType.first)) {
                        if (it->right != nullptr) it = findMin_(it->right);
                        else {
                            it = it->parent;
                        }
                    } else {
                        if (it->right != nullptr) {
                            it = findMin_(it->right);
                        } else {
                            map_node* temp = it;
                            while (it->parent->right == it) {
                                it = it->parent;
                                if (it->parent == nullptr) {
                                    return tmp;
                                }
                            }
                            it = it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMin_(it->right);
                    if (temp != nullptr) it = temp;
                    else it = it->parent;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator & operator++() {
                if (it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent->parent != nullptr) {
                    if (key_compare(it->valueType.first, it->parent->valueType.first)) {
                        if (it->right != nullptr) it = findMin_(it->right);
                        else {
                            it = it->parent;
                        }
                    } else {
                        if (it->right != nullptr) {
                            it = findMin_(it->right);
                        } else {
                            map_node* temp = it;
                            while (it->parent->right == it) {
                                it = it->parent;
                                if (it->parent == nullptr) {
                                    return *this;
                                }
                            }
                            it = it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMin_(it->right);
                    if (temp != nullptr) it = temp;
                    else it = it->parent;
                }
                return *this;
            }

            map_node* findMin_(map_node* node) {
                if (node == nullptr) return nullptr;
                map_node* tmp = node;
                while (tmp->left != nullptr) {
                    tmp = tmp->left;
                }
                return tmp;
            }

            map_node* findMax_(map_node* node) {
                if (node == nullptr) return nullptr;
                map_node* tmp = node;
                while (tmp->right != nullptr) {
                    tmp = tmp->right;
                }
                return tmp;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp(it);
                if (it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent == nullptr) {
                    if (it->left == nullptr && it->right == nullptr) throw Yuki::invalid_iterator();
                    it = findMax_(it->right);
                    return tmp;
                }
                if (it->parent->parent != nullptr) {
                    if (key_compare(it->valueType.first, it->parent->valueType.first)) {
                        if (it->left != nullptr) {
                            it = findMax_(it->left);
                        } else {
                            while (it->parent->left == it) {
                                it = it->parent;
                                if (it->parent->parent == nullptr) {
                                    throw Yuki::invalid_iterator();
                                }
                            }
                            it = it->parent;
                        }
                    } else {
                        if (it->left != nullptr) it = findMax_(it->left);
                        else {
                            it = it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMax_(it->left);
                    if (temp != nullptr) it = temp;
                    else {
                        throw Yuki::invalid_iterator();
                    }
                }
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator & operator--() {
                if (it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (it->parent == nullptr) {
                    if (it->left == nullptr && it->right == nullptr) throw Yuki::invalid_iterator();
                    it = findMax_(it->right);
                    return *this;
                }
                if (it->parent->parent != nullptr) {
                    if (key_compare(it->valueType.first, it->parent->valueType.first)) {
                        if (it->left != nullptr) {
                            it = findMax_(it->left);
                        }
                        else {
                            while (it->parent->left == it) {
                                it = it->parent;
                                if (it->parent == nullptr) {
                                    throw Yuki::invalid_iterator();
                                }
                            }
                            it = it->parent;
                        }
                    } else {
                        if (it->left != nullptr) it = findMax_(it->left);
                        else {
                            it = it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMax_(it->left);
                    if (temp != nullptr) it = temp;
                    else {
                        throw Yuki::invalid_iterator();
                    }
                }
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                return it->valueType;
            }
            bool operator==(const iterator &rhs) const {
                if (this->it == (rhs.it)) return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if (this->it == (rhs.const_it)) return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (*this == rhs) return false;
                else return true;
            }
            bool operator!=(const const_iterator &rhs) const {
                if (*this == rhs) return false;
                else return true;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                if (it->parent == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                else return &(it->valueType);
            }

        };
        class const_iterator {
            friend class map;
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            map_node* const_it;
            Compare key_compare;
        public:
            const_iterator() {
                const_it = nullptr;
            }
            explicit const_iterator(map_node* pointer, bool isEnd_ = false) {
                const_it = pointer;
            }
            const_iterator(const const_iterator &other) {
                const_it = other.const_it;
            }
            const_iterator(const iterator &other) {
                const_it = other.it;
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
            value_type & operator*() const {
                return const_it->valueType;
            }
            value_type* operator->() const noexcept {
                return &(const_it->valueType);
            }
            bool operator==(const iterator &rhs) const {
                if (this->const_it == rhs.it) return true;
                else return false;
            }
            bool operator==(const const_iterator &rhs) const {
                if (this->const_it == (rhs.const_it)) return true;
                else return false;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            /**
     * TODO iter++
     */
            const_iterator operator++(int) {
                const_iterator tmp(const_it);
                if (const_it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (const_it->parent == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (const_it->parent->parent != nullptr) {
                    if (key_compare(const_it->valueType.first, const_it->parent->valueType.first)) {
                        if (const_it->right != nullptr) const_it = findMin_(const_it->right);
                        else {
                            const_it = const_it->parent;
                        }
                    } else {
                        if (const_it->right != nullptr) {
                            const_it = findMin_(const_it->right);
                        } else {
                            while (const_it->parent->right == const_it) {
                                const_it = const_it->parent;
                                if (const_it->parent == nullptr) {
                                    return tmp;
                                }
                            }
                            const_it = const_it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMin_(const_it->right);
                    if (temp != nullptr) const_it = temp;
                    else const_it = const_it->parent;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator & operator++() {
                if (const_it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (const_it->parent == nullptr) {
                    throw Yuki::invalid_iterator();
                }

                if (const_it->parent->parent != nullptr) {
                    if (key_compare(const_it->valueType.first, const_it->parent->valueType.first)) {
                        if (const_it->right != nullptr) const_it = findMin_(const_it->right);
                        else {
                            const_it = const_it->parent;
                        }
                    } else {
                        if (const_it->right != nullptr) {
                            const_it = findMin_(const_it->right);
                        } else {
                            while (const_it->parent->right == const_it) {
                                const_it = const_it->parent;
                                if (const_it->parent == nullptr) {
                                    return *this;
                                }
                            }
                            const_it = const_it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMin_(const_it->right);
                    if (temp != nullptr) const_it = temp;
                    else const_it = const_it->parent;
                }
                return *this;
            }

            map_node* findMin_(map_node* node) {
                map_node* tmp = node;
                if (tmp == nullptr) return nullptr;
                while (tmp->left != nullptr) {
                    tmp = tmp->left;
                }
                return tmp;
            }

            map_node* findMax_(map_node* node) {
                map_node* tmp = node;
                if (tmp == nullptr) return nullptr;
                while (tmp->right != nullptr) {
                    tmp = tmp->right;
                }
                return tmp;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator tmp(const_it);
                if (const_it == nullptr) {
                    throw Yuki::invalid_iterator();
                }
                if (const_it->parent == nullptr) {
                    if (const_it->left == nullptr && const_it->right == nullptr) throw Yuki::invalid_iterator();
                    const_it = findMax_(const_it->right);
                    return tmp;
                }
                if (const_it->parent->parent != nullptr) {
                    if (key_compare(const_it->valueType.first, const_it->parent->valueType.first)) {
                        if (const_it->left != nullptr) {
                            const_it = findMax_(const_it->left);
                        }
                        else {
                            while (const_it->parent->left == const_it) {
                                const_it = const_it->parent;
                                if (const_it->parent == nullptr) {
                                    throw Yuki::invalid_iterator();
                                }
                            }
                            const_it = const_it->parent;
                        }
                    } else {
                        if (const_it->left != nullptr) const_it = findMax_(const_it->left);
                        else {
                            const_it = const_it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMax_(const_it->left);
                    if (temp != nullptr) const_it = temp;
                    else {
                        throw Yuki::invalid_iterator();
                    }
                }
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator & operator--() {
                if (const_it == nullptr) {
                    throw Yuki::invalid_iterator();
                }

                if (const_it->parent == nullptr) {
                    if (const_it->left == nullptr && const_it->right == nullptr) throw Yuki::invalid_iterator();
                    const_it = findMax_(const_it->right);
                    return *this;
                }
                if (const_it->parent != nullptr) {
                    if (key_compare(const_it->valueType.first, const_it->parent->valueType.first)) {
                        if (const_it->left != nullptr) {
                            const_it = findMax_(const_it->right);
                        }
                        else {
                            while (const_it->parent->left == const_it) {
                                const_it = const_it->parent;
                                if (const_it->parent == nullptr) {
                                    throw Yuki::invalid_iterator();
                                }
                            }
                            const_it = const_it->parent;
                        }
                    } else {
                        if (const_it->left != nullptr) const_it = findMax_(const_it->left);
                        else {
                            const_it = const_it->parent;
                        }
                    }
                } else {
                    map_node* temp = findMax_(const_it->left);
                    if (temp != nullptr) const_it = temp;
                    else {
                        throw Yuki::invalid_iterator();
                    }
                }
                return *this;
            }
        };
        /**
         * TODO two constructors
         */
        map() {
            length = 0;
            root = nullptr;
            head = alloc.allocate(1);
            head->parent = nullptr;
            head->left = nullptr;
            head->right = nullptr;
        }
        map(const map &other) {
            if (other.root == nullptr) return;
            root = new map_node(other.root->valueType, other.root->color);
            if (head == nullptr) head = alloc.allocate(1);
            head->left = root;
            head->right = root;
            head->parent = nullptr;
            root->parent = head;
            copy(root, other.root);
            length = other.length;
        }
        /**
         * TODO assignment operator
         */
        map & operator=(const map &other) {
            if (this == &other) return *this;
            clear();
            std::allocator_traits<std::allocator<map_node>>::deallocate(alloc, head, 1);
            head = nullptr;
            if (other.root == nullptr) return *this;
            root = new map_node(other.root->valueType, other.root->color);
            if (head == nullptr) head = alloc.allocate(1);
            head->left = root;
            head->right = root;
            head->parent = nullptr;
            root->parent = head;
            copy(root, other.root);
            length = other.length;
            return *this;
        }
        /**
         * TODO Destructors
         */
        ~map() {
            clear();
            //delete head;
            std::allocator_traits<std::allocator<map_node>>::deallocate(alloc, head, 1);
            head = nullptr;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            try {
                if (root == nullptr) throw Yuki::index_out_of_bound();
                map_node *tmp = find_(root, key);
                if (tmp == nullptr) throw Yuki::index_out_of_bound();
                else return tmp->valueType.second;
            } catch(Yuki::exception &error) {
                throw Yuki::index_out_of_bound();
            }
        }
        const T & at(const Key &key) const {
            if (root == nullptr) throw Yuki::index_out_of_bound();
            map_node* tmp = find_(root, key);
            if (tmp == nullptr) throw Yuki::index_out_of_bound();
            else return tmp->valueType.second;
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            map_node* obj = find_(root, key);
            if (obj != nullptr) return obj->valueType.second;
            else {
                return (insert(value_type(key, T())).first)->second;
            }
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            return at(key);
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if (root == nullptr) return iterator(head);
            map_node* begin_ = findMin(root);
            return iterator(begin_);
        }
        const_iterator cbegin() const {
            if (root == nullptr) return const_iterator(head);
            map_node* begin_ = findMin(root);
            return const_iterator(begin_);
        }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(head);
        }
        const_iterator cend() const {
            return const_iterator(head);
        }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            if (length == 0) return true;
            else return false;
        }
        /**
         * returns the number of elements.
         */
        size_t size() const {
            return length;
        }
        /**
         * clears the contents
         */
        void clear() {
            clear_(root);
            length = 0;
            root = nullptr;
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        Yuki::pair<iterator, bool> insert(const value_type &value) {
            if (root == nullptr) {
                root = new map_node(value, true);
                if (head == nullptr) {
                    head = alloc.allocate(1);
                    head->parent = nullptr;
                }
                root->parent = head;
                head->left = head->right = root;
                length++;
                return Yuki::pair<iterator, bool>(iterator(root), true);
            }
            // 新建结点
            auto* newNode = new map_node(value, false);
            // 找到插入的父节点
            map_node* tmp = root;
            map_node* Parent = tmp; // 目标父节点
            while (tmp != nullptr) {
                if (key_compare(tmp->valueType.first, value.first)) {
                    Parent = tmp;
                    tmp = tmp->right;
                } else if (key_compare(value.first, tmp->valueType.first)){
                    Parent = tmp;
                    tmp = tmp->left;
                }else {
                    delete newNode;
                    return Yuki::pair<iterator, bool> (iterator(tmp), false);
                }
            }

            newNode->parent = Parent;
            // 如果父节点为黑色
            if (key_compare(Parent->valueType.first, value.first)) Parent->right = newNode;
            else Parent->left = newNode;
            if (!Parent->color) balance(Parent, newNode);
            length++;
            return Yuki::pair<iterator, bool> (iterator(newNode), true);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos == end()) throw Yuki::index_out_of_bound();
            if (pos.it == nullptr) throw Yuki::index_out_of_bound();
            iterator DeleteIt = find(pos->first);
            map_node* D_node = DeleteIt.it;
            if (D_node == nullptr) throw Yuki::index_out_of_bound();
            if (D_node != pos.it) throw Yuki::index_out_of_bound();
            length--;
            // 用最左侧的最大值代替
            map_node* newNode;
            bool direction;// 左为假，右为真
            map_node* Parent;
            map_node* brother;
            if (D_node->right != nullptr && D_node->left != nullptr) {
                map_node* theNew;
                theNew = findMax(D_node->left);
                Parent = theNew->parent;
                if (key_compare(theNew->valueType.first, Parent->valueType.first)) {
                    brother = Parent->right;
                    direction = true;
                } else {
                    brother = Parent->left;
                    direction = false;
                }
                map_node* D_p = D_node->parent;
                bool c = D_node->color;
                D_node->color = theNew->color;
                theNew->color = c;
                map_node* D_l = D_node->left;
                map_node* D_r = D_node->right;
                if (D_p != head) {
                    if (key_compare(D_p->valueType.first, D_node->valueType.first)) D_p->right = theNew;
                    else D_p->left = theNew;
                } else {
                    root = theNew;
                    D_p->left = D_p->right = root;
                    root->parent = D_p;
                }
                if (theNew == D_l) {
                    D_node->left = theNew->left;
                    D_node->right = theNew->right;
                    theNew->parent = D_p;
                    theNew->right = D_r;
                    theNew->left = D_node;
                    D_node->parent = theNew;
                    D_r->parent = theNew;
                } else {
                    D_node->parent = Parent;
                    D_node->left = theNew->left;
                    D_node->right = theNew->right;
                    if (direction) Parent->left = D_node;
                    else Parent->right = D_node;
                    theNew->parent = D_p;
                    theNew->left = D_l;
                    D_l->parent = theNew;
                    theNew->right = D_r;
                    D_r->parent = theNew;
                }
                newNode = D_node;
                // 考虑删除根节点的情况
                // 但似乎也没有什么好考虑的
                Parent = newNode->parent;
            } else if (D_node->right == nullptr && D_node->left != nullptr) {
                newNode = D_node->left;
                if (D_node->parent == head) {
                    root = D_node->left;
                    root->color = true;
                    root->parent = head;
                    head->left = head->right = root;
                    delete D_node;
                    return;
                }
                newNode->parent = D_node->parent;
                newNode->color = D_node->color;
                if (key_compare(D_node->parent->valueType.first, newNode->valueType.first)) {
                    D_node->parent->right = newNode;
                } else D_node->parent->left = newNode;
                delete D_node;
                return;
            } else if (D_node->left == nullptr && D_node->right != nullptr) {
                newNode = D_node->right;
                if (D_node->parent == head) {
                    root = D_node->right;
                    root->color = true;
                    root->parent = head;
                    head->right = head->left = root;
                    delete D_node;
                    return;
                }
                newNode->parent = D_node->parent;
                newNode->color = D_node->color;
                if (key_compare(D_node->parent->valueType.first, newNode->valueType.first)) {
                    D_node->parent->right = newNode;
                } else D_node->parent->left = newNode;
                delete D_node;
                return;
            } else {
                newNode = D_node;
                if (D_node->parent == head) {
                    delete D_node;
                    root = nullptr;
                    head->left = head->right = root;
                    return;
                }
                Parent = newNode->parent;
                if (key_compare(newNode->valueType.first, Parent->valueType.first)) {
                    brother = Parent->right;
                    direction = true;
                } else {
                    brother = Parent->left;
                    direction = false;
                }
            }
            // 结点为叶结点
            if (newNode->right == nullptr && newNode->left != nullptr) {
                if (Parent->left == newNode) {
                    Parent->left = newNode->left;
                } else Parent->right = newNode->left;
                newNode->left->color = newNode->color;
                newNode->left->parent = newNode->parent;
                delete newNode;
                return;
            } else if (newNode->left == nullptr && newNode->right != nullptr) {
                if (Parent->left == newNode) Parent->left = newNode->right;
                else Parent->right = newNode->right;
                newNode->right->color = newNode->color;
                newNode->right->parent = newNode->parent;
                delete newNode;
                return;
            }
            if (!newNode->color) {
                delete newNode;
                if (direction) Parent->left = nullptr;
                else Parent->right = nullptr;
                return;
            }

            // 如果叶结点为黑色，那么它一定有兄弟结点
            // 如果叶结点为红色，则不一定有兄弟节点
            map_node* bro_l = brother->left;
            map_node* bro_r = brother->right;
            if (!Parent->color && bro_l == nullptr && bro_r == nullptr) {
                Erase1(Parent, brother);
                delete newNode;
                if (direction) Parent->left = nullptr;
                else Parent->right = nullptr;
                return;
            }
            if (!brother->color) {
                // 一定有两个叶结点
                Erase2(Parent, brother, direction);
                delete newNode;
                if (direction) Parent->left = nullptr;
                else Parent->right = nullptr;
                return;
            }
            if (Parent->color && brother->color && bro_l == nullptr && bro_r == nullptr) {
                if (direction) Parent->left = nullptr;
                else Parent->right = nullptr;
                Erase(Parent, brother, direction);
                delete newNode;
                return;
            }
            if (direction && bro_r != nullptr) {
                Erase3(Parent, brother);
                delete newNode;
                Parent->left = nullptr;
                return;
            }
            if (!direction && bro_l != nullptr) {
                Erase4(Parent, brother);
                delete newNode;
                Parent->right = nullptr;
                return;
            }
            if (direction && bro_r == nullptr) {
                Erase5(Parent, brother);
                delete newNode;
                Parent->left = nullptr;
                return;
            }
            if (!direction && bro_l == nullptr) {
                Erase6(Parent, brother);
                delete newNode;
                Parent->right = nullptr;
                return;
            }
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            map_node* root_ = root;
            if (find_(root_, key) == nullptr) return 0;
            else return 1;
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            map_node* Node = find_(root, key);
            if (Node == nullptr) return end();
            else return iterator(Node);
        }
        const_iterator find(const Key &key) const {
            map_node* Node = find_(root, key);
            if (Node == nullptr) return cend();
            else return const_iterator(Node);
        }
    private:
        map_node * find_(map_node *obj, const Key &key) const {
            if (obj == nullptr) return nullptr;
            if (key_compare(obj->valueType.first, key))  {
                return find_(obj->right, key);
            } else if (key_compare(key, obj->valueType.first)){
                return find_(obj->left, key);
            } else {
                return obj;
            }
        }
        void copy(map_node* newNode, const map_node* node) {
            if (node == nullptr) return;
            if (node->left != nullptr) {
                newNode->left = new map_node(node->left->valueType, node->left->color);
                newNode->left->parent = newNode;
            } else newNode->left = nullptr;
            if (node->right != nullptr) {
                newNode->right = new map_node(node->right->valueType, node->right->color);
                newNode->right->parent = newNode;
            } else newNode->right = nullptr;
            copy(newNode->left, node->left);
            copy(newNode->right, node->right);
        }
        void clear_(map_node* obj) {
            if (obj == nullptr) return;
            clear_(obj->left);
            obj->left = nullptr;
            clear_(obj->right);
            obj->right = nullptr;
            delete obj;

        }
        map_node* findMin(map_node* node) const  {
            if (node == nullptr) return nullptr;
            map_node* tmp = node;
            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }
            return tmp;
        }
        map_node* findMax(map_node* node) const {
            if (node == nullptr) return nullptr;
            map_node* tmp = node;
            while (tmp->right != nullptr) {
                tmp = tmp->right;
            }
            return tmp;
        }

        // 左旋，成为子节点的左子树
        void rotateL(map_node* obj) {
            if (obj == nullptr) return;
            map_node* grand = obj->parent;
            map_node* newParent = obj->right;
            if (newParent == nullptr) return;
            newParent->parent = grand;
            if (obj == root) {
                obj->right = newParent->left;
                if (newParent->left != nullptr) newParent->left->parent = obj;
                newParent->left = obj;
                root = newParent;
                root->parent = head;
                head->right = root;
                head->left = root;
                obj->parent = newParent;
                return;
            }
            if (key_compare(grand->valueType.first, obj->valueType.first)) {
                grand->right = newParent;
            } else if (key_compare(obj->valueType.first, grand->valueType.first)){
                grand->left = newParent;
            }
            obj->parent = newParent;
            obj->right = newParent->left;
            if (newParent->left != nullptr) newParent->left->parent = obj;
            newParent->left = obj;

        }
        void rotateR(map_node* obj) {
            if (obj == nullptr) return;
            map_node* grand = obj->parent;
            map_node* newParent = obj->left;
            if (newParent == nullptr) return;
            if (obj == root) {
                obj->left = newParent->right;
                if (newParent->right != nullptr) newParent->right->parent = obj;
                newParent->right= obj;
                obj->parent = newParent;
                root = newParent;
                root->parent = head;
                head->left = root;
                head->right = root;
                return;
            }
            if (key_compare(grand->valueType.first, obj->valueType.first)) {
                grand->right = newParent;
            } else if (key_compare(obj->valueType.first, grand->valueType.first)){
                grand->left = newParent;
            }
            newParent->parent = grand;
            obj->left = newParent->right;
            if (newParent->right != nullptr) newParent->right->parent = obj;
            obj->parent = newParent;
            newParent->right = obj;
        }

        void case2(map_node* p) {
            if (p->parent == nullptr) {
                map_node* newRoot = p->left;
                p->left = newRoot->right;
                if (newRoot->right != nullptr) newRoot->right->parent = p;
                newRoot->right = p;
                p->parent = newRoot;
                root = newRoot;
                root->parent = head;
                head->left = root;
                head->right = root;
                root->color = true;
                p->color = false;
                return;
            }
            rotateR(p);
            p->color = false;
            p->parent->color = true;
        }
        void case3(map_node* p) {
            if (p->parent->parent == nullptr) {
                map_node* oldRoot = p->parent;
                map_node* newRoot = p->right;
                p->right = newRoot->left;
                if (newRoot->left != nullptr) newRoot->left->parent = p;
                newRoot->left = p;
                p->parent = newRoot;
                oldRoot->left = newRoot->right;
                if (newRoot->right != nullptr) newRoot->right->parent = oldRoot;
                newRoot->right = oldRoot;
                oldRoot->parent = newRoot;
                root = newRoot;
                root->parent = head;
                head->left = root;
                head->right = root;
                root->color = true;
                oldRoot->color = false;
                p->color = false;
                return;
            }
            rotateL(p);
            rotateR(p->parent->parent);
            p->parent->color = true;
            if (p->parent->right != nullptr) p->parent->right ->color = false;
        }
        void case4(map_node* p) {
            if (p->parent == nullptr) {
                map_node* newRoot = p->right;
                p->right = newRoot->left;
                if(newRoot->left != nullptr) newRoot->left->parent = p;
                newRoot->left = p;
                p->parent = newRoot;
                root = newRoot;
                root->parent = head;
                head->left = root;
                head->right = root;
                root->color = true;
                p->color = false;
                return;
            }
            rotateL(p);
            p->color = false;
            p->parent->color = true;
        }
        void case5(map_node* p) {
            if (p->parent->parent == nullptr) {
                map_node* oldRoot = p->parent;
                map_node* newRoot = p->left;
                p->left = newRoot->right;
                if (newRoot->right != nullptr) newRoot->right->parent = p;
                newRoot->right = p;
                p->parent = newRoot;
                oldRoot->right = newRoot->left;
                if(newRoot->left != nullptr) newRoot->left->parent = oldRoot;
                newRoot->left = oldRoot;
                oldRoot->parent = newRoot;
                newRoot->color = true;
                p->color = false;
                oldRoot->color = false;
                root = newRoot;
                root->parent = head;
                head->left = root;
                head->right = root;
                return;
            }
            rotateR(p);
            rotateL(p->parent->parent);
            p->parent->color = true;
            p->parent->left->color = false;
        }

        void balance(map_node* Parent, map_node* cur) {
            if (Parent == head) {
                cur->color = true;
                return;
            }
            if (Parent->color == true) return;
            if (Parent == root) {
                Parent->color = true;// 根节点
                return;
            }
            map_node* grand = Parent->parent;
            map_node* uncle;
            if (key_compare(Parent->valueType.first, grand->valueType.first)) {
                // Parent为左子树
                uncle = grand->right;
                if (uncle != nullptr && !uncle->color) {
                    // recolor
                    Parent->color = true;
                    uncle->color = true;
                    grand->color = false;
                    root->color = true;
                    if (grand->parent->parent != nullptr) balance(grand->parent, grand);
                    else grand->color = true;
                } else {
                    if (key_compare(cur->valueType.first, Parent->valueType.first)) {
                        // case2 均为左子树
                        case2(grand);
                    } else {
                        case3(Parent);
                    }
                }
            } else {
                uncle = grand->left;
                if (uncle != nullptr && !uncle->color) {
                    Parent->color = true;
                    uncle->color = true;
                    grand->color = false;
                    root->color = true;
                    if (grand->parent->parent != nullptr) balance(grand->parent, grand);
                    else grand->color = true;
                } else {
                    if (key_compare(cur->valueType.first, Parent->valueType.first)) {
                        case5(Parent);
                    } else {
                        case4(grand);
                    }
                }
            }
        }

        void Erase1(map_node* Parent, map_node* brother) {
            Parent->color = true;
            brother->color = false;
        }

        void Erase2(map_node* Parent, map_node* brother, bool direction) {
            Parent->color = false;
            brother->color = true;
            if (direction) {
                rotateL(Parent);
                if (Parent->right->right != nullptr && !Parent->right->right->color) {
                    Erase3(Parent,Parent->right);
                    return;
                }
                if (Parent->right->left != nullptr && !Parent->right->left->color) {
                    Erase5(Parent, Parent->right);
                    return;
                }
                Erase1(Parent, Parent->right);
            } else {
                rotateR(Parent);
                if (Parent->left->left != nullptr && !Parent->left->left->color) {
                    Erase4(Parent, Parent->left);
                    return;
                }
                if (Parent->left->right != nullptr && !Parent->left->right->color) {
                    Erase6(Parent, Parent->left);
                    return;
                }
                Erase1(Parent, Parent->left);
            }
        }

        void Erase3(map_node* Parent, map_node* brother) {
            brother->right->color = true;
            bool tmp = Parent->color;
            Parent->color = brother->color;
            brother->color = tmp;
            rotateL(Parent);
        }

        void Erase4(map_node* Parent, map_node* brother) {
            brother->left->color = true;
            bool tmp = Parent->color;
            Parent->color = brother->color;
            brother->color = tmp;
            rotateR(Parent);
        }

        void Erase5(map_node* Parent, map_node* brother) {
            brother->color = false;
            brother->left->color = true;
            rotateR(brother);
            Erase3(Parent, Parent->right);
        }

        void Erase6(map_node* Parent, map_node* brother) {
            brother->color = false;
            brother->right->color = true;
            rotateL(brother);
            Erase4(Parent, Parent->left);
        }

        void Erase(map_node* Parent, map_node *brother, bool direction) {
            if (direction) Parent->right->color = false;
            else Parent->left->color = false;
            map_node* grand = Parent->parent;
            map_node* uncle;
            bool newDir;
            if (Parent == root) {
                brother->color = false;
                return;
            }
            if (key_compare(Parent->valueType.first, grand->valueType.first)) {
                newDir = true;
                uncle = grand->right;
            } else {
                newDir = false;
                uncle = grand->left;
            }
            map_node* un_r = uncle->right;
            map_node* un_l = uncle->left;
            if (grand->color && uncle->color) {
                if (un_r != nullptr && un_l != nullptr) {
                    if (un_r->color && un_l->color) {
                        Erase(grand, uncle, newDir);
                        return;
                    }
                    if (!un_r->color) {
                        if (newDir) Erase3(grand, uncle);
                        else Erase6(grand, uncle);
                    } else {
                        if (newDir) Erase5(grand, uncle);
                        else Erase4(grand, uncle);
                    }
                    return;
                }

            }
            if (newDir && un_r != nullptr && !un_r->color) {
                Erase3(grand, uncle);
                return;
            }
            if (!newDir && un_l != nullptr && !un_l->color) {
                Erase4(grand, uncle);
                return;
            }
            if (newDir && un_l != nullptr && !un_l->color) {
                Erase5(grand, uncle);
                return;
            }
            if (!newDir && un_r != nullptr && !un_r->color) {
                Erase6(grand, uncle);
                return;
            }
            if (!grand->color) {
                Erase1(grand, uncle);
                return;
            }
            if (!uncle->color) {
                Erase2(grand, uncle, newDir);
                return;
            }
        }
    };

}

#endif //TICKETSYSTEM_MAP_HPP
