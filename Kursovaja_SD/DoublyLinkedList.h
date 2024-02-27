#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

template <typename T>
class Node {
public:
    T data;
    Node* next;
    Node* prev;

    Node(T val) : data(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    std::mutex mutex;

    Node<T>* findMiddle(Node<T>* start, Node<T>* end);
    void merge(Node<T>* start, Node<T>* mid, Node<T>* end);
    void parallelMergeSortHelper(Node<T>* start, Node<T>* end);

public:
    DoublyLinkedList();
    ~DoublyLinkedList();
    void insertBack(T val);
    void insertFront(T val);
    void insertAfter(Node<T>* prevNode, T val);
    Node<T>* getHead() const;
    void display();
    void parallelMergeSort();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void removeAt(int index);
    void choice();
};
