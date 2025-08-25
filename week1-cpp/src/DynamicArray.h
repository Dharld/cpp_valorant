#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

class DynamicArray {
    int* data;      // pointer to the heap array
    int size;       // how many elements are stored
    int capacity;   // how much space is allocated

public:
    DynamicArray();                          // default constructor
    DynamicArray(const DynamicArray& other); // copy constructor
    ~DynamicArray();                         // destructor

    int getSize() const;
    void push_back(int value);
    int pop_back();
    void resize(int new_size);
    void clear();

    int& operator[](int index);
    const int& operator[](int index) const;
};

#endif // DYNAMICARRAY_H

