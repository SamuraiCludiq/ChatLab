#ifndef COMPATIBILITY_HPP
#define COMPATIBILITY_HPP

#ifdef _WIN32
#define WINDOWS(exp) \
    while (1) {      \
        exp          \
    };
#define LINUX(exp)
#else
#define WINDOWS(exp)
#define LINUX(exp) \
    while (1) {    \
        exp        \
    };
#endif

#endif /* !COMPATIBILITY_HPP */
