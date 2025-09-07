// stats/main.cpp
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

using std::cin;
using std::cout;

struct Options {
    bool quiet = false;
};

[[noreturn]] void usage(const char* prog, int code = 0) {
    std::cerr << "Usage: " << prog << " [--quiet] [-h|--help]\n";
    std::exit(code);
}

Options parse_args(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string_view a{argv[i]};
        if (a == "-h" || a == "--help") {
            usage(argv[0], 0);
        } else if (a == "--quiet") {
            opt.quiet = true;
        } else {
            throw std::invalid_argument("unknown option: " + std::string(a));
        }
    }
    return opt;
}

int main(int argc, char** argv) {
    Options opt;
    try {
        opt = parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        usage(argv[0], 2); // exit 2 on CLI error
    }

    // n: count, mean: running mean, M2: sum of squared deviations
    long long n = 0;
    double mean = 0.0;
    double M2 = 0.0;

    // Track min/max exactly as integers read
    long long vmin = std::numeric_limits<long long>::max();
    long long vmax = std::numeric_limits<long long>::min();

    long long x;
    while (cin >> x) {
        ++n;
        if (x < vmin) vmin = x;
        if (x > vmax) vmax = x;

        double dx = static_cast<double>(x) - mean;
        mean += dx / static_cast<double>(n);
        double dx2 = static_cast<double>(x) - mean;
        M2 += dx * dx2;
    }

    if (n == 0) {
        if (!opt.quiet) std::cerr << "no input values\n";
        return 1; // as specified: exit 1 on no data
    }

    double variance = M2 / static_cast<double>(n); // population variance

    // Print with sensible precision (matches examples like 2.5, 1.25)
    cout << std::setprecision(12)
         << "count=" << n
         << " min=" << vmin
         << " max=" << vmax
         << " mean=" << mean
         << " variance=" << variance
         << '\n';

    return 0;
}
