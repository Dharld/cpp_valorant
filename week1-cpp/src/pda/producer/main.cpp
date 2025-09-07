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

static bool parse_ll_strict(std::string_view s, long long& out) {
    // Accept only strict base-10 integers (no spaces, no suffix/prefix)
    const char* b = s.data();
    const char* e = b + s.size();
    auto [ptr, ec] = std::from_chars(b, e, out, 10);
    return ec == std::errc() && ptr == e;
}

int main(int argc, char** argv) {
    Options opt;
    try {
        opt = parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        usage(argv[0], 2);
    }

    unsigned long long emitted = 0;
    unsigned long long skipped = 0;

    std::string tok;
    while (cin >> tok) {
        long long v = 0;
        if (parse_ll_strict(tok, v)) {
            cout << v << '\n';
            ++emitted;
        } else {
            ++skipped;
        }
    }

    if (!opt.quiet) {
        std::cerr << "skipped: " << skipped << '\n';
    }

    if (emitted > 0) return 0;
    return 1;
}
