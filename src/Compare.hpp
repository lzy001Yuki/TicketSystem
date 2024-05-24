#ifndef TICKETSYSTEM_COMPARE_HPP
#define TICKETSYSTEM_COMPARE_HPP
#include<functional>
template <class T, class cmp = std::less<T>>
class Sort{
private:
    cmp comp;
    void merge(T arr[], int start, int mid, int end) {
        T result[end - start + 5];
        int k = 0;
        int i = start;
        int j = mid + 1;
        while (i <= mid && j <= end) {
            if (comp(arr[i], arr[j])){
                result[k++] = arr[i++];
            }
            else{
                result[k++] = arr[j++];
            }
        }
        if (i == mid + 1) {
            while(j <= end)
                result[k++] = arr[j++];
        }
        if (j == end + 1) {
            while (i <= mid)
                result[k++] = arr[i++];
        }
        for (j = 0, i = start ; j < k; i++, j++) {
            arr[i] = result[j];
        }
    }
public:
    void mergeSort(T arr[], int start, int end) {
        if (start >= end)
            return;
        int mid = ( start + end ) / 2;
        mergeSort(arr, start, mid);
        mergeSort(arr, mid + 1, end);
        merge(arr, start, mid, end);
    }
};
#endif //TICKETSYSTEM_COMPARE_HPP
