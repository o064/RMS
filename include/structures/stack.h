#ifndef RMS_STACK_H
#define RMS_STACK_H

#include <stdexcept>

template <class Type>
struct Node {
    Type data;
    Node<Type>* link;
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
        Node<Type>* newNode = new Node<Type>();
        newNode->data = value;
        newNode->link = topPtr;
        topPtr = newNode;
        ++counter;
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

    bool empty() const {
        return topPtr == nullptr;
    }

    int size() const {
        return counter;
    }
};

#endif // RMS_STACK_H
