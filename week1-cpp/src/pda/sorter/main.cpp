// sorter/main.cpp
#include "DynamicArray.h"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>   // std::move

using std::cout;
using std::cin;

struct Options {
    bool unique = false;
    std::optional<size_t> reserve;    
    bool quiet = false;
};

[[noreturn]] void usage(const char* prog, int code = 0) {
    std::cerr << "Usage: " << prog << " [--unique] [--reserve=N] [--quiet]\n"
                 "       " << prog << " [--unique] [--reserve N] [--quiet]\n";
    std::exit(code);
}

size_t parse_size_t(std::string_view s) {
    size_t v = 0;
    auto* b = s.data();
    auto* e = s.data() + s.size();
    auto [ptr, ec] = std::from_chars(b, e, v);
    if (ec != std::errc() || ptr != e) {
        throw std::invalid_argument("bad integer: " + std::string(s));    
    }
    return v;
}

Options parse_args(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string_view a{argv[i]};

        if (a == "-h" || a == "--help") {
            usage(argv[0], 0);
        } else if (a == "--unique") {
            opt.unique = true;
        } else if (a == "--quiet") {
            opt.quiet = true;
        } else if (a.rfind("--reserve=", 0) == 0) {
            opt.reserve = parse_size_t(a.substr(10));
        } else if (a == "--reserve") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("--reserve needs a value");
            }
            opt.reserve = parse_size_t(std::string_view{argv[++i]});
        } else {
            throw std::invalid_argument("unknown option: " + std::string(a));
        }
    }
    return opt;
}

int main(int argc, char** argv) {
    Options opts;
    try {
        opts = parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        usage(argv[0], 2); // exit 2 on CLI error
    }

    DynamicArray arr;
    if (opts.reserve.has_value()) {
        arr.reserve(opts.reserve.value());
    }

    int x;
    while (cin >> x) {
        arr.push_back(x);
    }

    if (arr.get_size() == 0) {
        if (!opts.quiet) {
            std::cerr << "no input values\n";
        }
        return 1; // exit 1 = no data
    }

    // sort ascending
    std::sort(arr.begin(), arr.end());

    // compress duplicates if --unique
    if (opts.unique) {
        auto new_end = std::unique(arr.begin(), arr.end());
        arr.resize(new_end - arr.begin());
    }

    // print result to stdout
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        cout << *it << '\n';
    }

    return 0; // success
}
