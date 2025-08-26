// main.cpp
#include "DynamicArray.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;

template <class F>
void section(const std::string& title, F&& f) {
    cout << "\n=== " << title << " ===\n";
    try { f(); }
    catch (const std::exception& e) { cout << "Exception: " << e.what() << "\n"; }
}

int main() {
    // 0) Sanity: empty construct
    section("Empty construct", [] {
        DynamicArray a;
        cout << "size=" << a.get_size() << " cap=" << a.get_capacity()
             << " reallocs=" << a.get_reallocation_count() << "\n";
        assert(a.get_size() == 0);
    });

    // 1) reserve() prevents reallocations
    section("reserve prevents reallocations", [] {
        DynamicArray a;
        a.reserve(10'000);
        cout << "after reserve: size=" << a.get_size()
             << " cap=" << a.get_capacity()
             << " reallocs=" << a.get_reallocation_count() << "\n";
        assert(a.get_capacity() == 10'000);
        int before = a.get_reallocation_count();

        for (int i = 0; i < 10'000; ++i) a.push_back(i);
        cout << "after pushes: size=" << a.get_size()
             << " cap=" << a.get_capacity()
             << " reallocs=" << a.get_reallocation_count() << "\n";
        assert(a.get_size() == 10'000);
        // Should be exactly 1 (the reserve itself)
        assert(a.get_reallocation_count() == before);
    });

    // 2) No reserve → multiple reallocations (likely)
    section("no reserve implies multiple reallocations", [] {
        DynamicArray b;
        for (int i = 0; i < 10'000; ++i) b.push_back(i);
        cout << "size=" << b.get_size()
             << " cap=" << b.get_capacity()
             << " reallocs=" << b.get_reallocation_count() << "\n";
        assert(b.get_size() == 10'000);
        // Expect > 1 realloc (doubling growth)
        assert(b.get_reallocation_count() > 1);
    });

    // 3) operator[] and at() behavior (including throwing)
    section("indexing and at()", [] {
        DynamicArray a;
        for (int i = 0; i < 5; ++i) a.push_back(i * 10);
        cout << "a[3]=" << a[3] << "\n";
        a[3] = 99;
        cout << "a[3] (after write)=" << a[3] << "\n";
        try {
            (void)a.at(5); // out of range
            assert(false && "expected out_of_range");
        } catch (const std::out_of_range&) {
            cout << "caught out_of_range on a.at(5)\n";
        }
    });

    // 4) Copy constructor / copy assignment are deep copies
    section("copy ctor and copy assignment are deep", [] {
        DynamicArray a;
        for (int i = 0; i < 6; ++i) a.push_back(i);

        DynamicArray c(a);           // copy ctor
        DynamicArray d; d = a;       // copy assign

        c[0] = 111;
        d[1] = 222;

        cout << "a[0]=" << a[0] << " a[1]=" << a[1] << "\n";
        cout << "c[0]=" << c[0] << " d[1]=" << d[1] << "\n";

        // a unchanged
        assert(a[0] == 0 && a[1] == 1);
        // c and d diverged from a
        assert(c[0] == 111);
        assert(d[1] == 222);
    });

    // 5) Move constructor: steal buffer; moved-from is empty but usable
    section("move constructor", [] {
        DynamicArray a;
        for (int i = 0; i < 8; ++i) a.push_back(10 + i);

        DynamicArray b(std::move(a));  // move-construct
        cout << "b.size=" << b.get_size() << " a.size=" << a.get_size() << "\n";
        // a should be in empty state and reusable
        assert(a.get_size() == 0);
        a.push_back(42);
        assert(a.get_size() == 1);
        assert(b.get_size() == 8);
    });

    // 6) Move assignment
    section("move assignment", [] {
        DynamicArray x;
        for (int i = 0; i < 5; ++i) x.push_back(i);
        DynamicArray y;
        for (int i = 0; i < 3; ++i) y.push_back(100 + i);

        y = std::move(x);
        cout << "y.size=" << y.get_size() << " x.size=" << x.get_size() << "\n";
        assert(y.get_size() == 5);
        assert(x.get_size() == 0);
        // y contents should be 0..4
        for (int i = 0; i < 5; ++i) assert(y[i] == i);
    });

    // 7) Self-ops: self-assign and self-move should not crash
    section("self assign / self move", [] {
        DynamicArray z;
        for (int i = 0; i < 4; ++i) z.push_back(i * 7);
        z = z;               // self copy-assign
        z = std::move(z);    // self move-assign
        cout << "z.size=" << z.get_size() << "\n";
        assert(z.get_size() == 4);
    });

    // 8) Iterators + algorithms (range-for, sort, accumulate, binary_search)
    section("iterators + algorithms", [] {
        DynamicArray a;
        for (int i = 5; i >= 1; --i) a.push_back(i); // 5,4,3,2,1

        // range-for mutate
        for (auto& x : a) x *= 2; // 10,8,6,4,2

        // sort ascending (requires begin/end correct)
        std::sort(a.begin(), a.end()); // 2,4,6,8,10
        int sum = std::accumulate(a.begin(), a.end(), 0);
        cout << "sorted sum=" << sum << " (expect 30)\n";
        assert(sum == 30);

        bool has8 = std::binary_search(a.begin(), a.end(), 8);
        bool has9 = std::binary_search(a.begin(), a.end(), 9);
        cout << "has8=" << has8 << " has9=" << has9 << "\n";
        assert(has8 && !has9);
    });

    // 9) resize up/down; clear; pop_back (including exception)
    section("resize / clear / pop_back", [] {
        DynamicArray a;
        a.resize(5); // should default-fill with zeros per your impl
        for (int i = 0; i < a.get_size(); ++i) assert(a[i] == 0);

        a.resize(8);
        assert(a.get_size() == 8);
        a[6] = 12;
        a[7] = 13;

        a.resize(3);  // shrink size only
        assert(a.get_size() == 3);

        // pop_back working
        int last = a.pop_back(); // pops index 2
        cout << "popped=" << last << "\n";
        assert(a.get_size() == 2);

        // clear keeps capacity, sets size to 0
        int cap_before = a.get_capacity();
        a.clear();
        assert(a.get_size() == 0);
        assert(a.get_capacity() == cap_before);

        // popping empty throws
        try {
            (void)a.pop_back();
            assert(false && "expected out_of_range from pop_back on empty");
        } catch (const std::out_of_range&) {
            cout << "caught out_of_range on pop_back() from empty\n";
        }
    });

    // 10) Tiny timing: reserve vs no-reserve (just to see a difference)
    section("micro-bench (reserve vs no-reserve)", [] {
        constexpr int N = 200'000;

        auto t0 = std::chrono::high_resolution_clock::now();
        DynamicArray a;
        a.reserve(N);
        for (int i = 0; i < N; ++i) a.push_back(i);
        auto t1 = std::chrono::high_resolution_clock::now();

        DynamicArray b;
        for (int i = 0; i < N; ++i) b.push_back(i);
        auto t2 = std::chrono::high_resolution_clock::now();

        auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        cout << "with reserve:   " << d1 << " ms, reallocs=" << a.get_reallocation_count() << "\n";
        cout << "without reserve:" << d2 << " ms, reallocs=" << b.get_reallocation_count() << "\n";
        assert(a.get_reallocation_count() <= 2); // reserve + maybe one from resize logic, but likely 1
        assert(b.get_reallocation_count() > a.get_reallocation_count());
    });

    cout << "\nAll tests passed ✅\n";
    return 0;
}
