#ifndef RMS_STACK_H
#define RMS_STACK_H

#include <stdexcept>

template <class T>
struct Node {
    T data;
    Node<T>* link;
    Node(const T& val =-99999, Node<T>* link_ptr= nullptr) : data(val), link(link_ptr) {}
};

template <class T>
class stack {
private:
    Node<T>* topPtr;
    int counter;

public:
    stack() : topPtr(nullptr), counter(0) {}

    ~stack() {
        while (!empty()) pop();
    }

    void push(const T& value) {
        topPtr = new Node<T>(value, topPtr);
        ++counter;
    }
    //copy constructor
    stack(const stack& other);
    // assignment overloading
    stack& operator=(stack other);
    friend void swap(stack<T>& first, stack<T>& second)  {
        using std::swap;
        swap(first.topPtr, second.topPtr);
        swap(first.counter, second.counter);
    }
    void pop() {
        if (empty())
            throw std::runtime_error("Stack underflow: pop() on empty stack");

        Node<T>* temp = topPtr;
        topPtr = topPtr->link;
        delete temp;
        --counter;
    }

    T& top() {
        if (empty())
            throw std::runtime_error("Stack underflow: top() on empty stack");

        return topPtr->data;
    }
    const T& top() const {
        if (empty())
            throw std::runtime_error("Stack underflow: top() on empty stack");
        return topPtr->data;
    }
    bool empty() const {
        return topPtr == nullptr;
    }

    int size() const {
        return counter;
    }
};
// --- Implementation of Copy Constructor ---
template <class T>
stack<T>::stack(const stack& other) : topPtr(nullptr), counter(0) {
    if (other.empty()) {
        return; //empty stack
    }

    Node<T>* otherCurrent = other.topPtr;
    Node<T>* lastNewNode = nullptr;

    Node<T>* newNode = new Node<T>();

    newNode->data = otherCurrent->data;
    newNode->link = nullptr;

    this->topPtr = newNode;
    lastNewNode = newNode;

    otherCurrent = otherCurrent->link;
    this->counter = 1;

    // Copy the rest of the nodes
    while (otherCurrent != nullptr) {
        Node<T>* newNode = new Node<T>();
        newNode->data = otherCurrent->data;
        newNode->link = nullptr;

        lastNewNode->link = newNode;
        lastNewNode = newNode;

        otherCurrent = otherCurrent->link;
        this->counter++;
    }

}

template <class T>
stack<T>& stack<T>::operator=(stack<T> other) {
    swap(*this, other);
    return *this;
}

#endif // RMS_STACK_H
