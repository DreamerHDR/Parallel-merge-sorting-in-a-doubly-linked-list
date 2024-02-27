#include "DoublyLinkedList.h"

int main() {
    system("chcp 1251");
    DoublyLinkedList<int> list;

    list.insertBack(12);
    list.insertBack(11);
    list.insertBack(13);
    list.insertBack(5);
    list.insertBack(6);
    list.insertBack(7);

    list.display();
    list.choice();

    return 0;
}
