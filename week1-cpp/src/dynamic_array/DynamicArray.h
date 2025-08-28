#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <initializer_list>
#include <ostream>
class DynamicArray {
    int* data;      // pointer to the heap array
    int size;       // how many elements are stored
    int capacity;  // how much space is allocated
    int reallocation_count;

public:
    DynamicArray();                          // default constructor
    DynamicArray(const DynamicArray& other); // copy constructor
    DynamicArray(DynamicArray&& other) noexcept;      // move constructor
    DynamicArray(std::initializer_list<int> list);
    DynamicArray(const int* first, const int* last);
    ~DynamicArray();                         // destructor

    int get_size() const; 
    int get_capacity() const;
    void push_back(int value);
    int pop_back();
    void resize(int new_size);
    void clear();
    int& at(int index);
    const int& at(int index) const;
    void reserve(int new_capacity);
    int get_reallocation_count() const;
    void reset_reallocation_count();
    void swap(DynamicArray& other) noexcept;
    void erase(int pos);
    void insert(int pos, int value);

    // Iterators
    using iterator = int*;
    using const_iterator = const int*;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    int& operator[](int index);
    const int& operator[](int index) const;
    DynamicArray& operator=(const DynamicArray& other);
    DynamicArray& operator=(DynamicArray&& other) noexcept;
    DynamicArray operator+(const DynamicArray& other) const;
    bool operator==(const DynamicArray& other) const;
    bool operator!=(const DynamicArray& other) const;
    friend std::ostream& operator<<(std::ostream& os, const DynamicArray& arr);
};

#endif // DYNAMICARRAY_H

