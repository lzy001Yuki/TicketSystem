#ifndef TICKETSYSTEM_VECTOR_HPP
#define TICKETSYSTEM_VECTOR_HPP
#include <climits>
#include <cstddef>
namespace Yuki {
    template <typename T>
    class vector {
    public:
        size_t length = 0;
        T **array = nullptr;// array是一个指针数组
        size_t capacity = 0;

        vector() {
            array = nullptr;
            capacity = 0;
            length = 0;
        }

        vector(const vector &other) {
            capacity = other.capacity;
            length = other.length;
            array = new T *[capacity];
            for (int i = 0; i < length; i++) {
                T data = *other.array[i];
                array[i] = new T(data);
            }
        }

        ~vector() {
            for (int i = 0; i < length; i++) {
                delete array[i];
            }
            delete[] array;
        }

        vector &operator=(const vector &other) {
            if (this == &other) return *this;
            for (int i = 0; i < length; i++) {
                delete array[i];
            }
            delete[] array;
            capacity = other.capacity;
            length = other.length;
            array = new T *[capacity];
            for (int i = 0; i < length; i++) {
                T data = *other.array[i];
                array[i] = new T(data);
            }
            return *this;
        }

        T &operator[](const size_t &pos) {
            return *array[pos];
        }

        const T &operator[](const size_t &pos) const {
            return *array[pos];
        }

        const T &front() const {
            if (array != nullptr) return *array[0];
        }

        const T &back() const {
            if (array != nullptr) return *array[length - 1];
        }

        bool empty() {
            if (length == 0) return true;
            else return false;
        }

        size_t size() {
            return length;
        }

        void clear() {
            for (int i = 0; i < length; i++) {
                delete array[i];
            }
            length = 0;
        }

        void shrink() {
            capacity /= 2;
            T **newArray = new T *[capacity];
            for (int i = 0; i < length; i++) {
                newArray[i] = array[i];
            }
            delete[] array;
            array = newArray;
        }

        void resize() {
            if (capacity == 0) capacity = 1;
            else capacity *= 2;
            T **newArray = new T *[capacity];
            for (int i = 0; i < length - 1; i++) {
                newArray[i] = array[i];
            }
            delete[] array;
            array = newArray;
        }

        void push_back(const T &value) {
            length++;
            if (length >= capacity) {
                resize();
            }
            array[length - 1] = new T(value);
        }

        void pop_back() {
            delete array[length - 1];
            length--;
            if (length <= capacity / 4) shrink();
        }
    };
}

#endif //TICKETSYSTEM_VECTOR_HPP
