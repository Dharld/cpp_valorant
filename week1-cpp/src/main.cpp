#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <sys/wait.h>

struct Options {
    bool unique = false;                 // forward to sorter
    bool quiet  = false;                 // forward to all
    std::optional<std::size_t> binw;     // forward to stats (when used)
};

[[noreturn]] void usage(const char* prog, int code = 0) {
    std::cerr << "Usage: " << prog << " [--unique] [--quiet] [--binw=N]\n"
                 "       " << prog << " [-h|--help]\n";
    std::exit(code);
}

static std::size_t parse_size_t(std::string_view s) {
    std::size_t v = 0;
    const char* b = s.data();
    const char* e = b + s.size();
    auto [ptr, ec] = std::from_chars(b, e, v);
    if (ec != std::errc() || ptr != e) {
        throw std::invalid_argument("bad integer: " + std::string(s));
    }
    return v;
}

static Options parse_args(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string_view a{argv[i]};
        if (a.empty()) continue;

        if (a == "-h" || a == "--help") {
            usage(argv[0], 0);
        } else if (a == "--unique") {
            opt.unique = true;
        } else if (a == "--quiet") {
            opt.quiet = true;
        } else if (a.rfind("--binw=", 0) == 0) {
            std::size_t w = parse_size_t(a.substr(7));
            if (w == 0) throw std::invalid_argument("--binw must be > 0");
            opt.binw = w;
        } else if (a == "--binw") {
            if (i + 1 >= argc) throw std::invalid_argument("--binw needs a value");
            std::size_t w = parse_size_t(std::string_view{argv[++i]});
            if (w == 0) throw std::invalid_argument("--binw must be > 0");
            opt.binw = w;
        } else {
            throw std::invalid_argument("unknown option: " + std::string(a));
        }
    }
    return opt;
}

static int status_code_or_signal(int st) {
    if (WIFSIGNALED(st)) return -WTERMSIG(st); // negative = signaled
    if (WIFEXITED(st))   return WEXITSTATUS(st);
    return 3;
}

int main(int argc, char** argv) {
    Options opt;
    try {
        opt = parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        usage(argv[0], 2); // exit(2) on CLI error
    }

    int p12[2], p23[2];
    if (pipe(p12) == -1 || pipe(p23) == -1) {
        perror("pipe");
        return 127;
    }

    // ----- Child A: producer -----
    pid_t a = fork();
    if (a == 0) {
        // stdout -> p12[1]
        close(p12[0]);
        if (dup2(p12[1], STDOUT_FILENO) == -1) { perror("dup2 producer"); _exit(127); }
        close(p12[1]);
        // not using p23
        close(p23[0]); close(p23[1]);

        if (opt.quiet) {
            char* const av[] = { const_cast<char*>("./producer"),
                                 const_cast<char*>("--quiet"),
                                 nullptr };
            execvp(av[0], av);
        } else {
            char* const av[] = { const_cast<char*>("./producer"), nullptr };
            execvp(av[0], av);
        }
        perror("execvp(producer)");
        _exit(127);
    } else if (a < 0) {
        perror("fork(producer)");
        close(p12[0]); close(p12[1]); close(p23[0]); close(p23[1]);
        return 127;
    }

    // ----- Child B: sorter -----
    pid_t b = fork();
    if (b == 0) {
        // stdin <- p12[0]; stdout -> p23[1]
        close(p12[1]); close(p23[0]);
        if (dup2(p12[0], STDIN_FILENO)  == -1) { perror("dup2 sorter stdin");  _exit(127); }
        if (dup2(p23[1], STDOUT_FILENO) == -1) { perror("dup2 sorter stdout"); _exit(127); }
        close(p12[0]); close(p23[1]);

        if (opt.unique && opt.quiet) {
            char* const av[] = { const_cast<char*>("./sorter"),
                                 const_cast<char*>("--unique"),
                                 const_cast<char*>("--quiet"),
                                 nullptr };
            execvp(av[0], av);
        } else if (opt.unique) {
            char* const av[] = { const_cast<char*>("./sorter"),
                                 const_cast<char*>("--unique"),
                                 nullptr };
            execvp(av[0], av);
        } else if (opt.quiet) {
            char* const av[] = { const_cast<char*>("./sorter"),
                                 const_cast<char*>("--quiet"),
                                 nullptr };
            execvp(av[0], av);
        } else {
            char* const av[] = { const_cast<char*>("./sorter"), nullptr };
            execvp(av[0], av);
        }
        perror("execvp(sorter)");
        _exit(127);
    } else if (b < 0) {
        perror("fork(sorter)");
        // Clean up and reap producer to avoid zombie
        close(p12[0]); close(p12[1]); close(p23[0]); close(p23[1]);
        int st; (void)waitpid(a, &st, 0);
        return 127;
    }

    // ----- Child C: stats -----
    pid_t c = fork();
    if (c == 0) {
        // stdin <- p23[0]; stdout = terminal
        // stats doesn’t use p12
        close(p12[0]); close(p12[1]); close(p23[1]);
        if (dup2(p23[0], STDIN_FILENO) == -1) { perror("dup2 stats stdin"); _exit(127); }
        close(p23[0]);

        if (opt.quiet && opt.binw.has_value()) {
            std::string bw = "--binw=" + std::to_string(*opt.binw);
            char* const av[] = { const_cast<char*>("./stats"),
                                 const_cast<char*>("--quiet"),
                                 const_cast<char*>(bw.data()),
                                 nullptr };
            execvp(av[0], av);
        } else if (opt.quiet) {
            char* const av[] = { const_cast<char*>("./stats"),
                                 const_cast<char*>("--quiet"),
                                 nullptr };
            execvp(av[0], av);
        } else if (opt.binw.has_value()) {
            std::string bw = "--binw=" + std::to_string(*opt.binw);
            char* const av[] = { const_cast<char*>("./stats"),
                                 const_cast<char*>(bw.data()),
                                 nullptr };
            execvp(av[0], av);
        } else {
            char* const av[] = { const_cast<char*>("./stats"), nullptr };
            execvp(av[0], av);
        }
        perror("execvp(stats)");
        _exit(127);
    } else if (c < 0) {
        perror("fork(stats)");
        close(p12[0]); close(p12[1]); close(p23[0]); close(p23[1]);
        int st; (void)waitpid(a, &st, 0); (void)waitpid(b, &st, 0);
        return 127;
    }

    // ----- Parent: close all FDs and wait -----
    close(p12[0]); close(p12[1]); close(p23[0]); close(p23[1]);

    int stA = 0, stB = 0, stC = 0;
    if (waitpid(a, &stA, 0) < 0) perror("waitpid(producer)");
    if (waitpid(b, &stB, 0) < 0) perror("waitpid(sorter)");
    if (waitpid(c, &stC, 0) < 0) perror("waitpid(stats)");

    // Exit policy:
    // - any child with exit 2 -> overall 2
    // - any child signaled    -> overall 3
    // - else stats' exit code (0 or 1)
    int ca = status_code_or_signal(stA);
    int cb = status_code_or_signal(stB);
    int cc = status_code_or_signal(stC);

    if (ca == 2 || cb == 2 || cc == 2) return 2;
    if (ca < 0 || cb < 0 || cc < 0)     return 3;

    // default: stats' exit (0 on data, 1 if empty stream)
    return cc;
}
