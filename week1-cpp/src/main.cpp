#include <iostream>
#include "DynamicArray.h"

void printDynamicArray(const DynamicArray& arr) {
    for (int i = 0; i < arr.getSize(); i++) {
        std::cout << arr[i] << " ";
    }

}

int main() { 
    DynamicArray arr;
    
    for (int i = 1; i <= 10; i++) {
        arr.push_back(i * i);
    }

    std::cout << "Array size: " << arr.getSize() << std::endl;
    printDynamicArray(arr); 

    // Copy array
    DynamicArray c = arr;
    printDynamicArray(c);
}

