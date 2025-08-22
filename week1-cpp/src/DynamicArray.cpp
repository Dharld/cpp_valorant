#include "DynamicArray.h"
#include <iostream>

DynamicArray::DynamicArray() : data(nullptr), size(0), capacity(0) {}

DynamicArray::DynamicArray(const DynamicArray& other)
    : data(nullptr), size(other.size), capacity(other.capacity) 
{
    if (capacity > 0) {
        data = new int[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
}

DynamicArray::~DynamicArray() {
    delete[] data;
}

int DynamicArray::getSize() const {
    return size;
}

void DynamicArray::push_back(int value) {
    if (size == capacity) {
        int new_capacity = (capacity == 0) ? 1 : capacity * 2;
        int* new_data = new int[new_capacity];

        for (int i = 0; i < size; i++) {
            new_data[i] = data[i];
        }

        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }
    data[size++] = value;
}

int& DynamicArray::operator[](int index) {
    return data[index];
}

const int& DynamicArray::operator[](int index) const {
    return data[index];
}
