#ifndef RMS_STACK_H
#define RMS_STACK_H

#include <stdexcept>

template <class Type>
struct Node {
    Type data;
    Node<Type>* link;
    Node(const Type& val =-99999, Node<Type>* link_ptr= nullptr) : data(val), link(link_ptr) {}
};

template <class Type>
class stack {
private:
    Node<Type>* topPtr;
    int counter;

public:
    stack() : topPtr(nullptr), counter(0) {}

    ~stack() {
        while (!empty()) pop();
    }

    void push(const Type& value) {
        topPtr = new Node<Type>(value, topPtr);
        ++counter;
    }
    //copy constructor
    stack(const stack& other);
    // assignment overloading
    stack& operator=(stack other);
    friend void swap(stack<Type>& first, stack<Type>& second)  {
        using std::swap;
        swap(first.topPtr, second.topPtr);
        swap(first.counter, second.counter);
    }
    void pop() {
        if (empty())
            throw std::runtime_error("Stack underflow: pop() on empty stack");

        Node<Type>* temp = topPtr;
        topPtr = topPtr->link;
        delete temp;
        --counter;
    }

    Type& top() {
        if (empty())
            throw std::runtime_error("Stack underflow: top() on empty stack");

        return topPtr->data;
    }
    const Type& top() const {
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
template <class Type>
stack<Type>::stack(const stack& other) : topPtr(nullptr), counter(0) {
    if (other.empty()) {
        return; //empty stack
    }

    Node<Type>* otherCurrent = other.topPtr;
    Node<Type>* lastNewNode = nullptr;

    Node<Type>* newNode = new Node<Type>();

    newNode->data = otherCurrent->data;
    newNode->link = nullptr;

    this->topPtr = newNode;
    lastNewNode = newNode;

    otherCurrent = otherCurrent->link;
    this->counter = 1;

    // Copy the rest of the nodes
    while (otherCurrent != nullptr) {
        Node<Type>* newNode = new Node<Type>();
        newNode->data = otherCurrent->data;
        newNode->link = nullptr;

        lastNewNode->link = newNode;
        lastNewNode = newNode;

        otherCurrent = otherCurrent->link;
        this->counter++;
    }

}

template <class Type>
stack<Type>& stack<Type>::operator=(stack<Type> other) {
    swap(*this, other);
    return *this;
}

#endif // RMS_STACK_H
