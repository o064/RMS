//
// Created by Omar on 12/11/2025.
//

#ifndef RMS_VECTOR_H
#define RMS_VECTOR_H

#include <iostream>
#include <algorithm> // for std::copy
#include <stdexcept> // for std::out_of_range
#include <initializer_list>

template <typename T>
class vector{
private:
    T* data;
    size_t current_size;
    size_t current_capacity;

    void resize(size_t new_capacity){
        //allocate new memory
        T* new_data = new T[new_capacity];
        //copy elements
        for(size_t i = 0; i < current_size; i++){
            new_data[i] = data[i];
        }
        //delete old memory
        delete[] data;
        //point to new mamory
        data = new_data;
        current_capacity = new_capacity;
    }
public:
    vector() : data(nullptr), current_size(0), current_capacity(0) {}
    ~vector(){
        delete[] data;
    }
    //copy constructor(Deep copy)
    vector(const vector& other)
            : data(new T[other.current_capacity]),
              current_size(other.current_size),
              current_capacity(other.current_capacity) {
        std::copy(other.data, other.data + current_size, data);
    }
    // initializer_list constructor
    vector(std::initializer_list<T> init)
            : data(new T[init.size()]),
              current_size(init.size()),
              current_capacity(init.size())
    {
        size_t i = 0;
        for (const T& val : init) {
            data[i++] = val;
        }
    }

    //copy assignment operator(Deep copy)
    vector& operator=(const vector& other){
        if(this != &other){
            delete[] data;
            current_size = other.current_size;
            current_capacity = other.current_capacity;
            data = new T[current_capacity];
            std::copy(other.data, other.data + current_size, data);
        }
        return *this;
    }
    //move constructor
    vector(vector&& other) noexcept
            : data(other.data), current_size(other.current_size), current_capacity(other.current_capacity){
        other.data = nullptr;
        other.current_size = 0;
        other.current_capacity = 0;
    }
    //move assignment constructor
    vector& operator=(vector&& other) noexcept{
        if(this != &other){
            delete[] data;
            data = other.data;
            current_size = other.current_size;
            current_capacity = other.current_capacity;
            other.data = nullptr;
            other.current_size = 0;
            other.current_capacity = 0;
        }
        return *this;
    }

    void push_back(const T& value){
        if (current_size == current_capacity){
            size_t new_capacity = (current_capacity == 0) ? 1 : current_capacity * 2;
            resize(new_capacity);
        }
        data[current_size] = value;
        current_size++;
    }

    void pop_back(){
        if (current_size > 0) {
            current_size--;
        }
    }

    size_t size() const{return current_size;}
    size_t capacity() const {return current_capacity;}

    T& operator[](size_t index){
        if (index >= current_size) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    const T& operator[](size_t index) const{
        if (index >= current_size) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    T* begin(){return data;}
    T* end(){return data + current_size;}

    const T* begin() const{return data;}
    const T* end() const {return data + current_size;}

    T& front(){
        if(current_size == 0) throw std::out_of_range("Vector is empty");
        return data[0];
    }
    T& back(){
        if(current_size == 0) throw std::out_of_range("Vector is empty");
        return data[current_size - 1];
    }
    bool empty() const{
        return current_size == 0;
    }
    void clear(){
        current_size = 0;
    }

    void insert(size_t index, const T& value){
        if(index > current_size){
            throw std::out_of_range("Index out of bounds");
        }
        if(current_size == current_capacity){
            size_t new_capacity = (current_capacity == 0) ? 1 : current_capacity * 2;
            resize(new_capacity);
        }
        for(size_t i = current_size; i > index; --i){
            data[i] = data[i - 1];
        }
        data[index] = value;
        current_size++;
    }

    void erase(size_t index){
        if(index >= current_size){
            throw std::out_of_range("Index out of bounds");
        }
        for(size_t i = index; i < current_size - 1; ++i){
            data[i] = data[i + 1];
        }
        current_size--;
    }


};

#endif //RMS_VECTOR_H
