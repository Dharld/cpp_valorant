#include "DynamicArray.h"
#include <iostream>
#include <new>
#include <stdexcept>

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

int DynamicArray::pop_back() {
   if (size == 0) {
        throw std::out_of_range("pop_back() called on an empty array"); 
   }
   return data[size--];
}

void DynamicArray::clear() {
    size = 0;
}

// In DynamicArray.cpp
void DynamicArray::resize(int new_size) {
    if (new_size < 0) throw std::invalid_argument("resize: negative size");

    // Case 1: no reallocation needed
    if (new_size <= capacity) {
        if (new_size > size) {
            for (int i = size; i < new_size; ++i) data[i] = 0; // default-fill
        }
        size = new_size;
        return;
    }

    // Case 2: need more capacity
    int new_cap = capacity == 0 ? new_size : capacity;
    while (new_cap < new_size) new_cap *= 2;  // grow (doubling strategy)

    int* new_data = new int[new_cap];

    // copy old elements
    for (int i = 0; i < size; ++i) new_data[i] = data[i];
    // default-fill the newly exposed range [size, new_size)
    for (int i = size; i < new_size; ++i) new_data[i] = 0;

    delete[] data;
    data = new_data;
    capacity = new_cap;
    size = new_size;
}

int& DynamicArray::operator[](int index) {
    if(index < size) {
        return data[index];
    }
    throw std::out_of_range("Tried to access an index out of range.");
}

const int& DynamicArray::operator[](int index) const {
    return data[index];
}
