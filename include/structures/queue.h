//
// Created by Omar on 12/14/2025.
//

#ifndef RMS_QUEUE_H
#define RMS_QUEUE_H

template <class T>
class queue {
    struct Node {
        T value;
        Node* next;
        Node(const T& v);
    };

    Node* frontPtr;
    Node* backPtr;
    int sz;

    void clear();
    void copy(const queue& q);

public:
    queue();
    ~queue();

    queue(const queue& q);
    queue& operator=(const queue& q);

    void push(const T& x);
    void pop();
    T& front();
    bool empty() const;
    int size() const;

    class iterator {
        Node* ptr;
    public:
        iterator(Node* p = nullptr);
        T& operator*();
        iterator& operator++();
        bool operator!=(const iterator& it) const;
    };

    class const_iterator {
        const Node* ptr;
    public:
        const_iterator(const Node* p = nullptr);
        const T& operator*() const;
        const_iterator& operator++();
        bool operator!=(const const_iterator& it) const;
    };

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

template <class T>
queue<T>::Node::Node(const T& v) : value(v), next(nullptr) {}

template <class T>
queue<T>::queue() : frontPtr(nullptr), backPtr(nullptr), sz(0) {}

template <class T>
queue<T>::~queue() {
    clear();
}

template <class T>
queue<T>::queue(const queue& q) : frontPtr(nullptr), backPtr(nullptr), sz(0) {
    copy(q);
}

template <class T>
queue<T>& queue<T>::operator=(const queue& q) {
    if (this != &q) {
        clear();
        copy(q);
    }
    return *this;
}

template <class T>
void queue<T>::clear() {
    while (!empty())
        pop();
}

template <class T>
void queue<T>::copy(const queue& q) {
    Node* cur = q.frontPtr;
    while (cur) {
        push(cur->value);
        cur = cur->next;
    }
}

template <class T>
void queue<T>::push(const T& x) {
    Node* n = new Node(x);
    if (empty())
        frontPtr = backPtr = n;
    else {
        backPtr->next = n;
        backPtr = n;
    }
    sz++;
}

template <class T>
void queue<T>::pop() {
    if (empty()) return;
    Node* temp = frontPtr;
    frontPtr = frontPtr->next;
    delete temp;
    sz--;
    if (empty())
        backPtr = nullptr;
}

template <class T>
T& queue<T>::front() {
    return frontPtr->value;
}

template <class T>
bool queue<T>::empty() const {
    return sz == 0;
}

template <class T>
int queue<T>::size() const {
    return sz;
}

template <class T>
queue<T>::iterator::iterator(Node* p) : ptr(p) {}

template <class T>
T& queue<T>::iterator::operator*() {
    return ptr->value;
}

template <class T>
typename queue<T>::iterator& queue<T>::iterator::operator++() {
    ptr = ptr->next;
    return *this;
}

template <class T>
bool queue<T>::iterator::operator!=(const iterator& it) const {
    return ptr != it.ptr;
}

template <class T>
queue<T>::const_iterator::const_iterator(const Node* p) : ptr(p) {}

template <class T>
const T& queue<T>::const_iterator::operator*() const {
    return ptr->value;
}

template <class T>
typename queue<T>::const_iterator& queue<T>::const_iterator::operator++() {
    ptr = ptr->next;
    return *this;
}

template <class T>
bool queue<T>::const_iterator::operator!=(const const_iterator& it) const {
    return ptr != it.ptr;
}

template <class T>
typename queue<T>::iterator queue<T>::begin() {
    return iterator(frontPtr);
}

template <class T>
typename queue<T>::iterator queue<T>::end() {
    return iterator(nullptr);
}

template <class T>
typename queue<T>::const_iterator queue<T>::begin() const {
    return const_iterator(frontPtr);
}

template <class T>
typename queue<T>::const_iterator queue<T>::end() const {
    return const_iterator(nullptr);
}


#endif //RMS_QUEUE_H
