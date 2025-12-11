//
// Created by Omar on 12/11/2025.
//

#ifndef RMS_MINHEAP_H
#define RMS_MINHEAP_H
#include <vector>
template <class T>
class MinHeap {
private:
    std::vector<T> h;

    void up(int i){
        while(i > 0){
            int p = (i-1)/2;
            if(h[i] < h[p]){
                swap(h[i], h[p]);
                i = p;
            } else break;
        }
    }

    void down(int i){
        int n = h.size();
        while(true){
            int l = 2*i+1, r = 2*i+2;
            int s = i;

            if(l < n && h[l] < h[s]) s = l;
            if(r < n && h[r] < h[s]) s = r;

            if(s != i){
                swap(h[i], h[s]);
                i = s;
            } else break;
        }
    }

    int findIdx(const T& v){
        for(int i=0;i<h.size();i++){
            if(h[i] == v) return i;
        }
        return -1;
    }

public:
    void push(T v){
        h.push_back(v);
        up(h.size()-1);
    }

    T top(){
        return h[0];
    }

    T pop(){
        T x = h[0];
        h[0] = h.back();
        h.pop_back();
        if(!h.empty()) down(0);
        return x;
    }

    bool remove(const T& v){
        int i = findIdx(v);
        if(i == -1) return false;

        h[i] = h.back();
        h.pop_back();

        if(i < h.size()){
            up(i);
            down(i);
        }

        return true;
    }

    bool empty(){
        return h.empty();
    }
};

#endif //RMS_MINHEAP_H
