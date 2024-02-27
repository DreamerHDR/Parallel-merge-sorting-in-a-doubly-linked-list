#include "DoublyLinkedList.h"

//����� ���������� �������� ����(�������� ������) ����� ����� ��������� �������(start � end)
template <typename T>
Node<T>* DoublyLinkedList<T>::findMiddle(Node<T>* start, Node<T>* end) {
    if (!start) return nullptr;
    Node<T>* slow = start;
    Node<T>* fast = start->next;
    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }
    //����� ��������� �� ������� ���� ������, �.�. fast ������ �������
    return slow;
}

//����� ����������� ���� ��������������� ������ ������ � ���� �����
template <typename T>
void DoublyLinkedList<T>::merge(Node<T>* start, Node<T>* mid, Node<T>* end) {
    Node<T>* left = start;
    Node<T>* right = mid->next;

    // ������� ��������� ������ ��� �������
    DoublyLinkedList<T> mergedList; 

    while (left != mid->next && right != end->next) {
        //���� l < r, �� l ����������� � ����� ������, ����� �������� r + ��������� ���������� �� ��������� ����
        if (left->data < right->data) {
            mergedList.insertBack(left->data);
            left = left->next;
        }
        else {
            mergedList.insertBack(right->data);
            right = right->next;
        }
    }

    //���� ��������� ��������� ����� ����� ��������, �� ���������� �������� ����������� � ��� ������� ������� ����
    while (left != mid->next) {
        mergedList.insertBack(left->data);
        left = left->next;
    }

    while (right != end->next) {
        mergedList.insertBack(right->data);
        right = right->next;
    }

    // �������� ��������������� ������� � �������� ������ �� ��������������� ������� �� mergedList
    Node<T>* temp = start;
    Node<T>* mergedCurrent = mergedList.getHead();
    while (temp != end->next) {
        temp->data = mergedCurrent->data;
        temp = temp->next;
        mergedCurrent = mergedCurrent->next;
    }

    std::cout << "������ ����� �������: ";
    display();
    std::cout << std::endl;
}

//����� ���������� ������������ ����������� ��������
template <typename T>
void DoublyLinkedList<T>::parallelMergeSortHelper(Node<T>* start, Node<T>* end) {
    //�������� ������� �� ����������� ��������
    if (start != end && start && end && start != end->next) {
        //���������� �������� ����
        Node<T>* mid = findMiddle(start, end);
        //�������� ���� �������. ��� leftThread(start � mid ��������� ����� ��������, this � �������� ��������� �� ������� ������)
        std::thread leftThread(&DoublyLinkedList<T>::parallelMergeSortHelper, this, start, mid);
        std::thread rightThread(&DoublyLinkedList<T>::parallelMergeSortHelper, this, mid->next, end);
        //��� ������ ����������� �������������, �������� ����� ������� �� ����������, ������ ��� ����� �������
        leftThread.join();
        rightThread.join();
        //������� ���� ������������� �������� ������
        merge(start, mid, end);
    }
}

//���������� ������, �������������� ���������
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr) {}

//���������� ������, ����������� ������ ������� ������
template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    while (head) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }
}

//����� ��� ���������� ������ ���� � ��������� ��������� � ����� ������
template <typename T>
void DoublyLinkedList<T>::insertBack(T val) {
    //������������� ������������� ������ � ������ �� ���������� �������
    std::lock_guard<std::mutex> lock(mutex);
    Node<T>* newNode = new Node<T>(val);
    if (!head) {
        head = tail = newNode;
    }
    else {
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
    }
}

//����� ��� ���������� ������ ���� � ��������� ��������� � ������ ������
template <typename T>
void DoublyLinkedList<T>::insertFront(T val) {
    std::lock_guard<std::mutex> lock(mutex);
    Node<T>* newNode = new Node<T>(val);
    if (!head) {
        head = tail = newNode;
    }
    else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}

//����� ��� ���������� ������ ���� � ��������� �������� ����� ��������� ����
template <typename T>
void DoublyLinkedList<T>::insertAfter(Node<T>* prevNode, T val) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!prevNode) {
        std::cout << "��������� ���� �� ����������." << std::endl;
        return;
    }
    Node<T>* newNode = new Node<T>(val);
    newNode->next = prevNode->next;
    if (prevNode->next)
        prevNode->next->prev = newNode;
    else
        tail = newNode;
    prevNode->next = newNode;
    newNode->prev = prevNode;
}

//����� ��� ��������� ��������� �� ������ ������.
template <typename T>
Node<T>* DoublyLinkedList<T>::getHead() const {
    return head;
}

template <typename T>
void DoublyLinkedList<T>::display() {
    Node<T>* current = head;
    while (current) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

//����� ��� ������� ������������ ���������� �������� ������
template <typename T>
void DoublyLinkedList<T>::parallelMergeSort() {
    //����� ���� ����� �������� ���������� �������� ��� �������, �� ��������� �������. ������ ������ ������ �����, ���� ������� ����� ����������
    std::lock_guard<std::mutex> lock(mutex);
    //���������� ����������, ������� � head � ���������� tail
    parallelMergeSortHelper(head, tail);
}

//����� ��� ���������� ����������� ������ � ����
template <typename T>
void DoublyLinkedList<T>::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "������ �������� ����� ��� ������." << std::endl;
        return;
    }

    Node<T>* current = head;
    while (current) {
        file << current->data << " ";
        current = current->next;
    }

    file.close();
    std::cout << "������ �������� � ����: " << filename << std::endl;
}

//����� ��� �������� ����������� ������ �� �����
template <typename T>
void DoublyLinkedList<T>::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "������ �������� ����� ��� ������." << std::endl;
        return;
    }

    // ������� ������� ������ ����� ���������
    while (head) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;

    T value;
    while (file >> value) {
        insertBack(value);
    }

    file.close();
    std::cout << "������ �������� �� �����: " << filename << std::endl;
}

//����� ��� �������� ���� �� ���������� �������
template <typename T>
void DoublyLinkedList<T>::removeAt(int index) {
    std::lock_guard<std::mutex> lock(mutex);
    Node<T>* current = head;
    int count = 0;
    while (current != nullptr && count != index) {
        current = current->next;
        ++count;
    }
    if (current == nullptr)
        return;

    if (current->prev)
        current->prev->next = current->next;
    else
        head = current->next;

    if (current->next)
        current->next->prev = current->prev;
    else
        tail = current->prev;

    delete current;
}

//����� ��� �������������� ������������ ���� ������ �������� � �������
template <typename T>
void DoublyLinkedList<T>::choice() {
    while (true) {
        int option;
        std::cout << "�������� ��������:" << std::endl;
        std::cout << "1. ���������� ������" << std::endl;
        std::cout << "2. ��������� ������ � ����" << std::endl;
        std::cout << "3. ��������� ������ �� �����" << std::endl;
        std::cout << "4. ����������� ������������� ������" << std::endl;
        std::cout << "5. �������� ������� � ����� ������" << std::endl;
        std::cout << "6. �������� ������� � ������ ������" << std::endl;
        std::cout << "7. �������� ������� ����� ���������� ����" << std::endl;
        std::cout << "8. ������� ������� �� �������" << std::endl;
        std::cout << "0. �����" << std::endl;
        std::cout << "��� �����: ";
        std::cin >> option;

        switch (option) {
        case 1:
            system("cls");
            std::cout << "������: ";
            display();
            system("pause");
            break;
        case 2:
        {
            system("cls");
            std::string filename;
            std::cout << "������� ��� ����� ��� ����������: ";
            std::cin >> filename;
            saveToFile(filename);
            system("pause");
        }
        break;
        case 3:
        {
            system("cls");
            std::string filename;
            std::cout << "������� ��� ����� ��� ��������: ";
            std::cin >> filename;
            loadFromFile(filename);
            system("pause");
        }
        break;
        case 4:
            system("cls");
            display();
            std::cout << "������������ ���������� ������..." << std::endl;
            std::cout << std::endl;
            parallelMergeSort();
            std::cout << "������ ������������." << std::endl;
            system("pause");
            break;
        case 5:
        {
            system("cls");
            T value;
            std::cout << "������� �������� ��� ���������� � ����� ������: ";
            std::cin >> value;
            insertBack(value);
            std::cout << "������� �������� � ����� ������." << std::endl;
            system("pause");
        }
        break;
        case 6:
        {
            system("cls");
            T value;
            std::cout << "������� �������� ��� ���������� � ������ ������: ";
            std::cin >> value;
            insertFront(value);
            std::cout << "������� �������� � ������ ������." << std::endl;
            system("pause");
        }
        break;
        case 7:
        {
            system("cls");
            T value;
            std::cout << "������� �������� ������ ��������: ";
            std::cin >> value;
            int position;
            std::cout << "������� ������� ����� ������� ������� �������� �������: ";
            std::cin >> position;
            Node<T>* current = head;
            while (current && position > 0) {
                current = current->next;
                position--;
            }
            insertAfter(current, value);
            std::cout << "������� �������� ����� ���������� ����." << std::endl;
            system("pause");
        }
        break;
        case 8:
        {
            system("cls");
            int index;
            std::cout << "������� ������ �������� ��� ��������: ";
            std::cin >> index;
            removeAt(index);
            std::cout << "������� ������ �� �������." << std::endl;
            system("pause");
        }
        break;
        case 0:
            std::cout << "����� �� ���������." << std::endl;
            return;
        default:
            std::cout << "�������� �����." << std::endl;
        }
        system("cls");
    }
}

template class DoublyLinkedList<int>;
template class DoublyLinkedList<float>;
template class DoublyLinkedList<double>;