// main.cpp
#include "DynamicArray.h"
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>   // std::move

using std::cout;
using std::cin;
using std::endl;

struct Stats {
    unsigned long long n = 0;
    long long minv = 0, maxv = 0;
    double mean = 0.0, M2 = 0.0;

    void add(long long x) {
        if (n == 0) {
            minv = maxv = x;
            mean = static_cast<double>(x);
            M2 = 0.0;
            n = 1;
            return;
        }

        if (x < minv) minv = x;
        if (x > maxv) maxv = x;

        ++n;
        double dx = static_cast<double>(x) - mean;
        mean += dx / static_cast<double>(n);
        double dx2 = static_cast<double>(x) - mean;
        M2 += dx * dx2;
    }

    double variance() const {
        return (n == 0) ? 0.0 : (M2 / static_cast<double>(n));
    }

};

struct Options {
    std::optional<size_t> binw;   // histogram bin width
    bool quiet = false;
};

void usage(const char* prog, int code = 0) {
    std::cerr << "Usage: " << prog << " [--binw=N] [--quiet]\n";
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
        }
        else if (a == "--quiet") {
            opt.quiet = true;
        }
        else if (a.rfind("--binw=", 0) == 0) {
            size_t w = parse_size_t(a.substr(7));
            if (w <= 0) throw std::invalid_argument("--binw must be > 0");
            opt.binw = w;
        }
        else if (a == "--binw") {
            if (i + 1 >= argc) throw std::invalid_argument("--binw needs a value");
            size_t w = parse_size_t(std::string_view(argv[++i]));
            if (w <= 0) throw std::invalid_argument("--binw must be > 0");
            opt.binw = w;
        }
        else {
            throw std::invalid_argument("unknown option: " + std::string(a));
        }
    }

    return opt;
}

int main(int argc, char** argv) {
    // Parse arguments
    Options opts;
    try {
        opts = parse_args(argc, argv);
    } catch(const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        usage(argv[0], 2);
    }


    Stats S;
    std::string tok;

    while(cin >> tok) {
        long long val = 0;
        auto* s = tok.data();
        auto* e = tok.data() + tok.size();

        auto [ptr, ec] = std::from_chars(s, e, val);

        if (ec == std::errc() && ptr == e) {
            S.add(val);
        } // Else silently fail the operation
    }

    if (S.n == 0) return 1;  // exact spec: no output, exit 1

    std::cout.setf(std::ios::fmtflags(0), std::ios::floatfield); // defaultfloat
    std::cout << std::setprecision(12);
    std::cout << "count=" << S.n
              << " min="   << S.minv
              << " max="   << S.maxv
              << " mean="  << S.mean
              << " variance=" << S.variance()
              << '\n';

    return 0; 

}
