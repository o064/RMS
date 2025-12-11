//
// Created by Omar on 12/11/2025.
//

#ifndef RMS_LIST_H
#define RMS_LIST_H
#include <iostream>
#include <functional>
#include <cassert>

template<typename T>
class List {
private:
    struct Node {
        T data;
        Node *prev;
        Node *next;
        Node(const T &v) : data(v), prev(nullptr), next(nullptr) {}
    };

    Node *head;
    Node *tail;
    size_t sz;

public:
    class iterator {
        friend class List;
    private:
        Node *node;
        explicit iterator(Node *n) : node(n) {}
    public:
        iterator() : node(nullptr) {}
        T& operator*() const { return node->data; }
        T* operator->() const { return std::addressof(node->data); }
        iterator& operator++() { if (node) node = node->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { if (node) node = node->prev; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
        bool operator==(const iterator &o) const { return node == o.node; }
        bool operator!=(const iterator &o) const { return node != o.node; }
    };

    class reverse_iterator {
    private:
        Node *node;
        explicit reverse_iterator(Node *n) : node(n) {}
        friend class List;
    public:
        reverse_iterator() : node(nullptr) {}
        T& operator*() const { return node->data; }
        reverse_iterator& operator++() { if (node) node = node->prev; return *this; }
        reverse_iterator operator++(int) { reverse_iterator tmp = *this; ++(*this); return tmp; }
        bool operator==(const reverse_iterator &o) const { return node == o.node; }
        bool operator!=(const reverse_iterator &o) const { return node != o.node; }
    };

    List() : head(nullptr), tail(nullptr), sz(0) {}
    ~List() { clear(); }

    List(const List&) = delete;
    List& operator=(const List&) = delete;

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    T& front() { assert(head); return head->data; }
    const T& front() const { assert(head); return head->data; }
    T& back() { assert(tail); return tail->data; }
    const T& back() const { assert(tail); return tail->data; }

    void push_front(const T &val) {
        Node *n = new Node(val);
        n->next = head;
        if (head) head->prev = n;
        head = n;
        if (!tail) tail = head;
        ++sz;
    }

    void push_back(const T &val) {
        Node *n = new Node(val);
        n->prev = tail;
        if (tail) tail->next = n;
        tail = n;
        if (!head) head = tail;
        ++sz;
    }

    void pop_front() {
        assert(head);
        Node *n = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
        delete n;
        --sz;
    }

    void pop_back() {
        assert(tail);
        Node *n = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr;
        delete n;
        --sz;
    }

    iterator insert(iterator pos, const T &val) {
        Node *posNode = pos.node;
        if (!posNode) {
            push_back(val);
            return iterator(tail);
        }
        Node *n = new Node(val);
        n->next = posNode;
        n->prev = posNode->prev;
        posNode->prev = n;
        if (n->prev) n->prev->next = n;
        else head = n;
        ++sz;
        return iterator(n);
    }

    iterator erase(iterator pos) {
        Node *n = pos.node;
        assert(n);
        iterator nextIt(n->next);
        if (n->prev) n->prev->next = n->next;
        else head = n->next;
        if (n->next) n->next->prev = n->prev;
        else tail = n->prev;
        delete n;
        --sz;
        return nextIt;
    }

    void clear() {
        Node *cur = head;
        while (cur) {
            Node *nx = cur->next;
            delete cur;
            cur = nx;
        }
        head = tail = nullptr;
        sz = 0;
    }

    void resize(size_t new_size, const T &value = T()) {
        while (sz < new_size) push_back(value);
        while (sz > new_size) pop_back();
    }

    void remove(const T &value) {
        for (auto it = begin(); it != end(); ) {
            if (*it == value) it = erase(it);
            else ++it;
        }
    }

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }

    reverse_iterator rbegin() { return reverse_iterator(tail); }
    reverse_iterator rend() { return reverse_iterator(nullptr); }

    void reverse() {
        Node *cur = head;
        Node *tmp = nullptr;
        while (cur) {
            tmp = cur->prev;
            cur->prev = cur->next;
            cur->next = tmp;
            cur = cur->prev;
        }
        tmp = head;
        head = tail;
        tail = tmp;
    }

    void merge(List &other) {
        if (this == &other) return;
        Node dummy(T());
        Node *last = &dummy;
        Node *a = head;
        Node *b = other.head;

        while (a && b) {
            if (b->data < a->data) {
                Node *bn = b->next;
                b->prev = last;
                last->next = b;
                b->next = nullptr;
                last = b;
                b = bn;
            } else {
                Node *an = a->next;
                a->prev = last;
                last->next = a;
                a->next = nullptr;
                last = a;
                a = an;
            }
        }
        while (a) {
            Node *an = a->next;
            a->prev = last;
            last->next = a;
            a->next = nullptr;
            last = a;
            a = an;
        }
        while (b) {
            Node *bn = b->next;
            b->prev = last;
            last->next = b;
            b->next = nullptr;
            last = b;
            b = bn;
        }

        Node *newHead = dummy.next;
        if (newHead) newHead->prev = nullptr;
        Node *newTail = newHead;
        while (newTail && newTail->next) newTail = newTail->next;

        head = newHead;
        tail = newTail;

        sz = 0;
        for (Node *p = head; p; p = p->next) ++sz;

        other.head = other.tail = nullptr;
        other.sz = 0;
    }

private:
    Node* split(Node* start) {
        if (!start) return nullptr;
        Node *slow = start, *fast = start->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        Node *second = slow->next;
        slow->next = nullptr;
        if (second) second->prev = nullptr;
        return second;
    }

    Node* merge_sorted(Node* a, Node* b) {
        Node dummy(T());
        Node *tailLocal = &dummy;
        while (a && b) {
            if (a->data <= b->data) {
                tailLocal->next = a;
                a->prev = tailLocal;
                a = a->next;
                tailLocal = tailLocal->next;
                tailLocal->next = nullptr;
            } else {
                tailLocal->next = b;
                b->prev = tailLocal;
                b = b->next;
                tailLocal = tailLocal->next;
                tailLocal->next = nullptr;
            }
        }
        while (a) {
            tailLocal->next = a;
            a->prev = tailLocal;
            a = a->next;
            tailLocal = tailLocal->next;
            tailLocal->next = nullptr;
        }
        while (b) {
            tailLocal->next = b;
            b->prev = tailLocal;
            b = b->next;
            tailLocal = tailLocal->next;
            tailLocal->next = nullptr;
        }
        Node *res = dummy.next;
        if (res) res->prev = nullptr;
        return res;
    }

    Node* merge_sort(Node* start) {
        if (!start || !start->next) return start;
        Node *second = split(start);
        Node *l1 = merge_sort(start);
        Node *l2 = merge_sort(second);
        return merge_sorted(l1, l2);
    }

public:
    void sort() {
        if (!head || !head->next) return;
        head = merge_sort(head);
        tail = head;
        while (tail && tail->next) tail = tail->next;
        sz = 0;
        for (Node *p = head; p; p = p->next) ++sz;
    }

    void print_forward() const {
        for (Node *p = head; p; p = p->next) std::cout << p->data << " ";
        std::cout << "\n";
    }

    void print_backward() const {
        for (Node *p = tail; p; p = p->prev) std::cout << p->data << " ";
        std::cout << "\n";
    }
};

#endif //RMS_LIST_H
