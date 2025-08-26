#include "DynamicArray.h"
#include <iostream>
#include <new>
#include <stdexcept>

DynamicArray::DynamicArray() : data(nullptr), size(0), capacity(0), reallocation_count(0) {}

DynamicArray::DynamicArray(const DynamicArray& other)
    : data(nullptr), size(other.size), capacity(other.capacity), reallocation_count(0)
{
    if (capacity > 0) {
        data = new int[capacity];
        ++reallocation_count;  // Initial allocation counts as reallocation
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
}

DynamicArray::DynamicArray(DynamicArray&& other) {
    swap(other);
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

DynamicArray::~DynamicArray() {
    delete[] data;
}

int DynamicArray::get_size() const {
    return size;
}

int DynamicArray::get_capacity() const {
    return capacity;
}

int DynamicArray::get_reallocation_count() const {
    return reallocation_count;
}

void DynamicArray::reset_reallocation_count() {
    reallocation_count = 0;
}

void DynamicArray::push_back(int value) {
    if (size == capacity) {
        int new_capacity = (capacity == 0) ? 1 : capacity * 2;
        int* new_data = new int[new_capacity];
        ++reallocation_count;  // Increment when buffer actually changes
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
   return data[--size];  // Fixed: should be --size, not size--
}

void DynamicArray::clear() {
    size = 0;
}

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
    
    // Case 2: need more capacity - reallocation required
    int new_cap = capacity == 0 ? new_size : capacity;
    while (new_cap < new_size) new_cap *= 2;  // grow (doubling strategy)
    
    int* new_data = new int[new_cap];
    ++reallocation_count;  // Increment when buffer actually changes
    
    // copy old elements
    for (int i = 0; i < size; ++i) new_data[i] = data[i];
    // default-fill the newly exposed range [size, new_size)
    for (int i = size; i < new_size; ++i) new_data[i] = 0;
    
    delete[] data;
    data = new_data;
    capacity = new_cap;
    size = new_size;
}

int& DynamicArray::at(int index) {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

const int& DynamicArray::at(int index) const {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

void DynamicArray::reserve(int new_capacity) {
    if (new_capacity <= capacity) return;  // No-op for shrink/same size
    
    int* new_data = new int[new_capacity];  // May throw - strong guarantee
    ++reallocation_count;  // Only increment when buffer actually changes
    
    // Copy existing elements
    for (int i = 0; i < size; i++) {
        new_data[i] = data[i];
    }
    
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

void DynamicArray::swap(DynamicArray& other) noexcept  {
    int temp_size = size;
    size = other.size;
    other.size = temp_size;

    int temp_capacity = capacity;
    capacity = other.capacity;
    other.capacity = temp_capacity;

    int* temp_data = data;
    data = other.data;
    other.data = temp_data;
}

DynamicArray::iterator DynamicArray::begin() {
    return data;
};

DynamicArray::const_iterator DynamicArray::begin() const {
    return data;
};

DynamicArray::const_iterator DynamicArray::cbegin() const {
    return data;
};

DynamicArray::iterator DynamicArray::end() {
    return data + size;
};

DynamicArray::const_iterator DynamicArray::end() const {
    return data + size;
};

DynamicArray::const_iterator DynamicArray::cend() const {
    return data + size;
};


int& DynamicArray::operator[](int index) {
    if(index < size) {
        return data[index];
    }
    throw std::out_of_range("Tried to access an index out of range.");
}

const int& DynamicArray::operator[](int index) const {
    return data[index];
}

DynamicArray& DynamicArray::operator=(const DynamicArray& other) {
    if (this == &other) {
        return *this;
    }
    
    delete[] data;
    size = other.size;
    capacity = other.capacity;
    
    if (capacity > 0) {
        data = new int[capacity];
        ++reallocation_count;  // Increment when buffer actually changes
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    } else {
        data = nullptr;
    }
    
    return *this;
}

DynamicArray& DynamicArray::operator=(DynamicArray&& other) {
    if (this == &other) return *this;

    swap(other);

    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;

    return *this;
}
