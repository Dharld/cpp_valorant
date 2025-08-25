#include <iostream>
#include "DynamicArray.h"

void printArray(const DynamicArray& arr) {
    std::cout << "Size = " << arr.getSize() << ": [ ";
    for (int i = 0; i < arr.getSize(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "]\n";
}

int main() {
    DynamicArray arr;

    // Fill with 1..5
    for (int i = 1; i <= 5; i++) arr.push_back(i);
    std::cout << "Initial array:\n";
    printArray(arr);

    // Shrink
    arr.resize(3);
    std::cout << "After resize(3):\n";
    printArray(arr);

    // Grow within capacity
    arr.resize(6);
    std::cout << "After resize(6):\n";
    printArray(arr);

    // Grow beyond capacity
    arr.resize(20);
    std::cout << "After resize(20):\n";
    printArray(arr);

    return 0;
}
