//Author: demon1991yl@gmail.com

#ifndef FDEMO_UTILS_UTILS_ALG_H_
#define FDEMO_UTILS_UTILS_ALG_H_

#include <cstdio>
namespace fdemo {
namespace utils{
    
template<typename T>
class SortSet {

public:
    virtual ~SortSet() {}
    SortSet() {}

    void BubbleSort(T sort_array[], int arr_len) {
        //sort
        for (int i=0;i<arr_len;i++){
            for (int j=0;j<arr_len-i-1;j++) {
                if (sort_array[j] > sort_array[j+1]) {
                    //swap
                    T tmp;
                    tmp = sort_array[j+1];
                    sort_array[j+1] = sort_array[j];
                    sort_array[j] = tmp;
                }
            }
        }
    }

    void InsertSort(T sort_array[], int arr_len) {
        for(int i=1; i<arr_len;i++) {
            T cur = sort_array[i];
            for (int j=i-1;j>=0;j--) {
                if(cur < sort_array[j]) {
                    sort_array[j+1] = sort_array[j];
                    sort_array[j] = cur;
                }
            }
        }
    }

    void SelectSort(T sort_array[], int arr_len) {
        for (int i=0;i<arr_len;i++) {
            int sel_pos = i;
            for (int j=i+1;j<arr_len;j++){
                if (sort_array[sel_pos] > sort_array[j]) {
                    sel_pos = j;
                }
            }
            //swap
            T tmp;
            tmp = sort_array[sel_pos];
            sort_array[sel_pos] = sort_array[i];
            sort_array[i] = tmp;
        }
    }

    void MergeSort(T sort_array[], int left, int right) {
        if (left < right) {
            int mid = left + (right-left)/2;
            printf("left:%d, mid:%d, right:%d\n", left, mid, right);
            MergeSort(sort_array, left, mid);
            MergeSort(sort_array, mid+1, right);
            Merge(sort_array, left, mid, right);
        }
    }

    void QuickSort(T sort_array[], int start, int end) {
        if(start < end) {
            int tmp_position = QuickSort_Partition(sort_array, start, end);
            QuickSort(sort_array, start, tmp_position-1);
            QuickSort(sort_array, tmp_position+1, end);
        }
    }

private:
    void Merge(T sort_array[], int left, int mid, int right) {
        T *t = new T[right-left+1];
        int i = left;
        int j = mid+1;
        int k = 0;
        while(i<=mid && j <=right) {
            if (sort_array[i] < sort_array[j]) {
                t[k++] = sort_array[i++];
            } else {
                t[k++] = sort_array[j++];
            }
        }
        if (i == mid+1) {
            while (j <= right) {
                t[k++] = sort_array[j++];
            }
        } else {
            while(i <= mid) {
                t[k++] = sort_array[i++];
            }
        }
        for (int i=left,k=0;i<=right;i++,k++) {
            sort_array[i] = t[k];
        }

        delete []t;
    }

    int QuickSort_Partition(T sort_array[], int start, int end) {
        int i = start-1;   
        T x = sort_array[end];
        for(int j = i+1;j<end;j++) {
            if (sort_array[j] < x ) {
                i++;
                T tmp = sort_array[i];
                sort_array[i] = sort_array[j];
                sort_array[j] =  tmp; 
            }
        }
        T tmp = sort_array[i+1];
        sort_array[i+1] = x;
        sort_array[end] = tmp;
        return i+1;
    }

};
    

} //namespace utils
} //namespace fdemo

#endif
