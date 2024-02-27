#include "DoublyLinkedList.h"

//Метод нахождения среднего узла(середины списка) между двумя заданными узадами(start и end)
template <typename T>
Node<T>* DoublyLinkedList<T>::findMiddle(Node<T>* start, Node<T>* end) {
    if (!start) return nullptr;
    Node<T>* slow = start;
    Node<T>* fast = start->next;
    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }
    //Будет указывать на средний узел списка, т.к. fast прошел быстрее
    return slow;
}

//Метод объединения двух отсортированных частей списка в одну часть
template <typename T>
void DoublyLinkedList<T>::merge(Node<T>* start, Node<T>* mid, Node<T>* end) {
    Node<T>* left = start;
    Node<T>* right = mid->next;

    // Создаем временный список для слияния
    DoublyLinkedList<T> mergedList; 

    while (left != mid->next && right != end->next) {
        //Если l < r, то l добавляется в конец списка, иначе значение r + указатели сдвигаются на следующий узел
        if (left->data < right->data) {
            mergedList.insertBack(left->data);
            left = left->next;
        }
        else {
            mergedList.insertBack(right->data);
            right = right->next;
        }
    }

    //Если указатель достигает конца своей половины, то оставшиеся элементы добавляются в том порядке котором идут
    while (left != mid->next) {
        mergedList.insertBack(left->data);
        left = left->next;
    }

    while (right != end->next) {
        mergedList.insertBack(right->data);
        right = right->next;
    }

    // Заменяем отсортированный участок в исходном списке на отсортированный участок из mergedList
    Node<T>* temp = start;
    Node<T>* mergedCurrent = mergedList.getHead();
    while (temp != end->next) {
        temp->data = mergedCurrent->data;
        temp = temp->next;
        mergedCurrent = mergedCurrent->next;
    }

    std::cout << "Список после слияния: ";
    display();
    std::cout << std::endl;
}

//Метод сортировки параллельным рекурсивным слиянием
template <typename T>
void DoublyLinkedList<T>::parallelMergeSortHelper(Node<T>* start, Node<T>* end) {
    //Проверка условия на продолжение рекурсии
    if (start != end && start && end && start != end->next) {
        //Нахождение среднего узла
        Node<T>* mid = findMiddle(start, end);
        //Создание двух потоков. Для leftThread(start и mid опредляют левую половину, this в качестве указателя на текущий объект)
        std::thread leftThread(&DoublyLinkedList<T>::parallelMergeSortHelper, this, start, mid);
        std::thread rightThread(&DoublyLinkedList<T>::parallelMergeSortHelper, this, mid->next, end);
        //Оба потока запускаются параллелельно, основной поток ожидает их завершения, прежде чем начнёт слияние
        leftThread.join();
        rightThread.join();
        //Слияние двух сортированных участков списка
        merge(start, mid, end);
    }
}

//Констуктор класса, инициализирует указатели
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr) {}

//Деструктор класса, освобождает память занятую узлами
template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    while (head) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }
}

//Метод для добавления нового узла с указанным значением в конец списка
template <typename T>
void DoublyLinkedList<T>::insertBack(T val) {
    //Предотвращает одновременный доступ к списку из нескольких потоков
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

//Метод для добавления нового узла с указанным значением в начало списка
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

//Метод для добавления нового узла с указанным значение после заданного узла
template <typename T>
void DoublyLinkedList<T>::insertAfter(Node<T>* prevNode, T val) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!prevNode) {
        std::cout << "Указанный узел не существует." << std::endl;
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

//Метод для получения указателя на начало списка.
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

//Метод для запуска параллельной сортировки слиянием списка
template <typename T>
void DoublyLinkedList<T>::parallelMergeSort() {
    //Когда один поток начинает выполнение операции над списком, он блокирует мьютекс. Другие потоки должны ждать, пока мьютекс будет освобожден
    std::lock_guard<std::mutex> lock(mutex);
    //Вызывается сортировка, начиная с head и заканчивая tail
    parallelMergeSortHelper(head, tail);
}

//Метод для сохранения содержимого списка в файл
template <typename T>
void DoublyLinkedList<T>::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для записи." << std::endl;
        return;
    }

    Node<T>* current = head;
    while (current) {
        file << current->data << " ";
        current = current->next;
    }

    file.close();
    std::cout << "Список сохранен в файл: " << filename << std::endl;
}

//Метод для загрузки содержимого списка из файла
template <typename T>
void DoublyLinkedList<T>::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для чтения." << std::endl;
        return;
    }

    // Очищаем текущий список перед загрузкой
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
    std::cout << "Список загружен из файла: " << filename << std::endl;
}

//Метод для удаления узла по указанному индексу
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

//Метод для предоставления пользователю меню выбора действий с списком
template <typename T>
void DoublyLinkedList<T>::choice() {
    while (true) {
        int option;
        std::cout << "Выберите действие:" << std::endl;
        std::cout << "1. Отобразить список" << std::endl;
        std::cout << "2. Сохранить список в файл" << std::endl;
        std::cout << "3. Загрузить список из файла" << std::endl;
        std::cout << "4. Параллельно отсортировать список" << std::endl;
        std::cout << "5. Добавить элемент в конец списка" << std::endl;
        std::cout << "6. Добавить элемент в начало списка" << std::endl;
        std::cout << "7. Добавить элемент после указанного узла" << std::endl;
        std::cout << "8. Удалить элемент по индексу" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Ваш выбор: ";
        std::cin >> option;

        switch (option) {
        case 1:
            system("cls");
            std::cout << "Список: ";
            display();
            system("pause");
            break;
        case 2:
        {
            system("cls");
            std::string filename;
            std::cout << "Введите имя файла для сохранения: ";
            std::cin >> filename;
            saveToFile(filename);
            system("pause");
        }
        break;
        case 3:
        {
            system("cls");
            std::string filename;
            std::cout << "Введите имя файла для загрузки: ";
            std::cin >> filename;
            loadFromFile(filename);
            system("pause");
        }
        break;
        case 4:
            system("cls");
            display();
            std::cout << "Параллельная сортировка списка..." << std::endl;
            std::cout << std::endl;
            parallelMergeSort();
            std::cout << "Список отсортирован." << std::endl;
            system("pause");
            break;
        case 5:
        {
            system("cls");
            T value;
            std::cout << "Введите значение для добавления в конец списка: ";
            std::cin >> value;
            insertBack(value);
            std::cout << "Элемент добавлен в конец списка." << std::endl;
            system("pause");
        }
        break;
        case 6:
        {
            system("cls");
            T value;
            std::cout << "Введите значение для добавления в начало списка: ";
            std::cin >> value;
            insertFront(value);
            std::cout << "Элемент добавлен в начало списка." << std::endl;
            system("pause");
        }
        break;
        case 7:
        {
            system("cls");
            T value;
            std::cout << "Введите значение нового элемента: ";
            std::cin >> value;
            int position;
            std::cout << "Введите позицию после которой следует вставить элемент: ";
            std::cin >> position;
            Node<T>* current = head;
            while (current && position > 0) {
                current = current->next;
                position--;
            }
            insertAfter(current, value);
            std::cout << "Элемент добавлен после указанного узла." << std::endl;
            system("pause");
        }
        break;
        case 8:
        {
            system("cls");
            int index;
            std::cout << "Введите индекс элемента для удаления: ";
            std::cin >> index;
            removeAt(index);
            std::cout << "Элемент удален по индексу." << std::endl;
            system("pause");
        }
        break;
        case 0:
            std::cout << "Выход из программы." << std::endl;
            return;
        default:
            std::cout << "Неверный выбор." << std::endl;
        }
        system("cls");
    }
}

template class DoublyLinkedList<int>;
template class DoublyLinkedList<float>;
template class DoublyLinkedList<double>;